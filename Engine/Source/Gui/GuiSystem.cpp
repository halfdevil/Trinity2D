#include "Gui/GuiSystem.h"
#include "Gui/GuiRenderer.h"
#include "Gui/Gui.h"
#include "Gui/Widget.h"
#include "Gui/WidgetEvent.h"
#include "Graphics/RenderPass.h"
#include "Graphics/RenderTarget.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	bool GuiSystem::create(RenderTarget& renderTarget, ResourceCache& cache)
	{
		mRenderer = std::make_unique<GuiRenderer>();
		if (!mRenderer->create(renderTarget, cache, kTexturedShader, kColoredShader))
		{
			LogError("GuiRenderer::create() failed with shader: '%s' and '%s'", kTexturedShader, kColoredShader);
			return true;
		}
		
		return true;
	}

	void GuiSystem::destroy()
	{
		mGui = nullptr;
		mRenderer = nullptr;
	}

	void GuiSystem::setGui(Gui& gui)
	{
		mGui = &gui;
	}

	void GuiSystem::setProjection(float x, float y, float width, float height)
	{
		mProjection = glm::ortho(x, width, y, height);
	}

	void GuiSystem::update(float deltaTime)
	{
		if (mGui != nullptr)
		{
			mGui->getRoot()->update(deltaTime);
		}
	}

	void GuiSystem::draw(const RenderPass& renderPass)
	{		
		draw(renderPass, mProjection);
	}

	void GuiSystem::draw(const RenderPass& renderPass, const glm::mat4& viewProj)
	{
		if (mGui != nullptr)
		{
			mRenderer->begin(viewProj);
			mGui->getRoot()->draw(*mRenderer);
			mRenderer->end(renderPass);
		}
	}

	void GuiSystem::mouseMove(float x, float y)
	{
		WidgetEvent evnt;
		evnt.getData().mousePosition = glm::vec2{ x, y };

		if (mGui != nullptr)
		{
			onMouseMove(evnt, mGui->getRoot());
		}
	}

	void GuiSystem::mouseDown(int32_t button)
	{
		WidgetEvent evnt;
		evnt.getData().mouseButton = button;

		if (mGui != nullptr)
		{
			onMouseDown(evnt, mGui->getRoot());
		}
	}

	void GuiSystem::mouseUp(int32_t button)
	{
		if (mFocusedWidget != nullptr)
		{
			WidgetEvent evnt;
			evnt.setType(WidgetEventType::MouseUp);
			evnt.getData().mouseButton = button;

			mFocusedWidget->handleEvent(evnt);
		}
	}

	void GuiSystem::keyDown(int32_t key)
	{
		if (mFocusedWidget != nullptr)
		{
			WidgetEvent evnt;
			evnt.setType(WidgetEventType::KeyDown);
			evnt.getData().key = key;

			mFocusedWidget->handleEvent(evnt);
		}
	}

	void GuiSystem::keyUp(int32_t key)
	{
		if (mFocusedWidget != nullptr)
		{
			WidgetEvent evnt;
			evnt.setType(WidgetEventType::KeyUp);
			evnt.getData().key = key;

			mFocusedWidget->handleEvent(evnt);
		}
	}

	bool GuiSystem::onMouseMove(WidgetEvent& evnt, Widget* widget)
	{
		auto result{ false };
		const auto& position = evnt.getData().mousePosition;
		const auto& children = widget->getChildren();

		for (auto* child : children)
		{
			if (onMouseMove(evnt, child))
			{
				result = true;
			}
		}

		if (widget->isPointOver(position.x, position.y))
		{
			if (!widget->isMouseOver())
			{
				widget->setMouseOver(true);
				evnt.setType(WidgetEventType::MouseEnter);
				widget->handleEvent(evnt);
			}

			if (!result)
			{
				evnt.setType(WidgetEventType::MouseMove);
				result = widget->handleEvent(evnt);
			}
		}
		else
		{
			if (widget->isMouseOver())
			{
				widget->setMouseOver(false);
				evnt.setType(WidgetEventType::MouseLeave);
				widget->handleEvent(evnt);
			}
		}
		
		return result;
	}

	bool GuiSystem::onMouseDown(WidgetEvent& evnt, Widget* widget)
	{
		auto result{ false };
		auto* oldFocusedWidget = mFocusedWidget;
		const auto& children = widget->getChildren();

		for (auto* child : children)
		{
			if (onMouseDown(evnt, child))
			{
				result = true;
			}
		}

		if (!result)
		{
			if (widget->isMouseOver())
			{
				evnt.setType(WidgetEventType::MouseDown);
				if (widget->handleEvent(evnt))
				{
					mFocusedWidget = widget;
					result = true;
				}

				if (result)
				{
					if (oldFocusedWidget != mFocusedWidget)
					{
						if (oldFocusedWidget != nullptr)
						{
							evnt.setType(WidgetEventType::LostFocus);
							oldFocusedWidget->handleEvent(evnt);
						}

						if (mFocusedWidget != nullptr)
						{
							evnt.setType(WidgetEventType::GotFocus);
							mFocusedWidget->handleEvent(evnt);
						}
					}
				}
			}
		}

		return result;
	}
}