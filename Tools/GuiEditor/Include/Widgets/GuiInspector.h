#pragma once

#include "Widgets/Inspector.h"

namespace Trinity
{
	class Gui;
	class Widget;

	class GuiInspector : public Inspector
	{
	public:

		GuiInspector() = default;
		virtual ~GuiInspector() = default;

		GuiInspector(const GuiInspector&) = delete;
		GuiInspector& operator = (const GuiInspector&) = delete;

		GuiInspector(GuiInspector&&) = default;
		GuiInspector& operator = (GuiInspector&&) = default;

		virtual void setGui(Gui& gui);
		virtual void setSelectedWidget(Widget* widget);

	protected:

		virtual void drawInspector() override;

	protected:

		Gui* mGui{ nullptr };
		Widget* mSelectedWidget{ nullptr };
		std::string mSelectedWidgetName;
	};
}