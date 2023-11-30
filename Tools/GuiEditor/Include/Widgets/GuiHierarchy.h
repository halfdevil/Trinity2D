#pragma once

#include "Core/EditorWidget.h"
#include "Core/Observer.h"
#include "Widgets/Menu.h"
#include <string>
#include <memory>

namespace Trinity
{
	class Gui;
	class Widget;

	class GuiHierarchy : public EditorWidget
	{
	public:

		GuiHierarchy() = default;
		virtual ~GuiHierarchy() = default;

		GuiHierarchy(const GuiHierarchy&) = delete;
		GuiHierarchy& operator = (const GuiHierarchy&) = delete;

		GuiHierarchy(GuiHierarchy&&) = default;
		GuiHierarchy& operator = (GuiHierarchy&&) = default;

		Widget* getCurrentWidget() const
		{
			return mCurrentWidget;
		}

		virtual void setGui(Gui& gui);
		virtual void draw() override;

	protected:

		virtual void drawWidget(Widget* widget, bool defaultOpen = false);

	public:

		Observer<Widget*> onSelectWidget;

	protected:

		Gui* mGui{ nullptr };
		Widget* mCurrentWidget{ nullptr };
	};
}