#pragma once

#include "Widgets/Viewport.h"

namespace Trinity
{
	class Gui;
	class Widget;
	class GuiSystem;

	class GuiViewport : public Viewport
	{
	public:

		GuiViewport() = default;
		virtual ~GuiViewport() = default;

		GuiViewport(const GuiViewport&) = delete;
		GuiViewport& operator = (const GuiViewport&) = delete;

		GuiViewport(GuiViewport&&) = default;
		GuiViewport& operator = (GuiViewport&&) = default;

		virtual bool create(EditorResources& resources);
		virtual void destroy() override;

		virtual void setGui(Gui& gui);
		virtual void setSelectedWidget(Widget* widget);
		virtual void drawContent(float deltaTime) override;

	protected:

		Gui* mGui{ nullptr };
		Widget* mSelectedWidget{ nullptr };
		std::unique_ptr<GuiSystem> mGuiSystem{ nullptr };
	};
}