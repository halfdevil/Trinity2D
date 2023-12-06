#include "Widgets/GuiViewport.h"
#include "Gui/Gui.h"
#include "Gui/Widget.h"
#include "Gui/GuiSystem.h"
#include "Gui/GuiRenderer.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderPass.h"
#include "Core/EditorTheme.h"
#include "Core/EditorResources.h"
#include "Core/EditorGizmo.h"
#include "Core/EditorCamera.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"

namespace Trinity
{
	bool GuiViewport::create(EditorResources& resources)
	{
		if (!Viewport::create(resources))
		{
			LogError("Viewport::create() failed");
			return false;
		}

		mGuiSystem = std::make_unique<GuiSystem>();
		if (!mGuiSystem->create(*mFrameBuffer, *resources.getResourceCache()))
		{
			LogError("GuiSystem::create() failed");
			return false;
		}

		return true;
	}

	void GuiViewport::destroy()
	{
		Viewport::destroy();
		mGuiSystem = nullptr;
	}

	void GuiViewport::setGui(Gui& gui)
	{
		mGui = &gui;
		mSelectedWidget = mGui->getRoot();
		mGuiSystem->setGui(gui);
	}

	void GuiViewport::setSelectedWidget(Widget* widget)
	{
		mSelectedWidget = widget;
	}

	void GuiViewport::drawContent(float deltaTime)
	{
		mRenderPass->begin(*mFrameBuffer);

		if (mCamera != nullptr)
		{
			mGuiSystem->draw(*mRenderPass, mCamera->getViewProj());
		}

		mRenderPass->end();
	}
}