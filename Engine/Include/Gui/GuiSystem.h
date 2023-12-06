#pragma once

#include "Core/Singleton.h"
#include <memory>
#include <string>
#include <typeindex>
#include "glm/glm.hpp"

namespace Trinity
{
	class Gui;
	class Widget;
	class GuiRenderer;
	class RenderTarget;
	class RenderPass;
	class ResourceCache;
	class WidgetEvent;

	class GuiSystem : public Singleton<GuiSystem>
	{
	public:

		static constexpr const char* kTexturedShader = "/Assets/Engine/Shaders/Textured.wgsl";
		static constexpr const char* kColoredShader = "/Assets/Engine/Shaders/Colored.wgsl";

		GuiSystem() = default;
		virtual ~GuiSystem() = default;

		GuiSystem(const GuiSystem&) = delete;
		GuiSystem& operator = (const GuiSystem&) = delete;

		GuiSystem(GuiSystem&&) = default;
		GuiSystem& operator = (GuiSystem&&) = default;

		GuiRenderer* getRenderer() const
		{
			return mRenderer.get();
		}

		virtual bool create(RenderTarget& renderTarget, ResourceCache& cache);
		virtual void destroy();

		virtual void setGui(Gui& gui);
		virtual void setProjection(float x, float y, float width, float height);
		virtual void update(float deltaTime);

		virtual void draw(const RenderPass& renderPass);
		virtual void draw(const RenderPass& renderPass, const glm::mat4& viewProj);

		virtual void mouseMove(float x, float y);
		virtual void mouseDown(int32_t button);
		virtual void mouseUp(int32_t button);
		virtual void keyDown(int32_t key);
		virtual void keyUp(int32_t key);

	protected:

		virtual bool onMouseMove(WidgetEvent& evnt, Widget* widget);
		virtual bool onMouseDown(WidgetEvent& evnt, Widget* widget);

	protected:

		Gui* mGui{ nullptr };
		Widget* mFocusedWidget{ nullptr };
		glm::mat4 mProjection;
		std::unique_ptr<GuiRenderer> mRenderer{ nullptr };
	};
}