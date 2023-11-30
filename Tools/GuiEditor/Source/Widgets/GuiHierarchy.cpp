#include "Widgets/GuiHierarchy.h"
#include "Gui/Gui.h"
#include "Gui/Widget.h"
#include "IconsFontAwesome6.h"

namespace Trinity
{
	void GuiHierarchy::setGui(Gui& gui)
	{
		mGui = &gui;
		mCurrentWidget = mGui->getRoot();
	}

	void GuiHierarchy::draw()
	{
		if (!isEnabled())
		{
			return;
		}

		ImGui::Begin(mTitle.c_str(), &mEnabled);
		{
			if (mGui != nullptr && mGui->getRoot() != nullptr)
			{
				drawWidget(mGui->getRoot(), true);
			}

			ImGui::End();
		}
	}

	void GuiHierarchy::drawWidget(Widget* widget, bool defaultOpen)
	{
		auto flags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_SpanFullWidth;

		if (defaultOpen)
		{
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		std::string label = ICON_FA_VECTOR_SQUARE " ";
		label.append(widget->getName());

		auto& children = widget->getChildren();
		if (children.size() == 0)
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		if (widget == mCurrentWidget)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool hasOpen = ImGui::TreeNodeEx(widget, flags, "%s", label.c_str());
		if (ImGui::IsItemClicked())
		{
			mCurrentWidget = widget;
			onSelectWidget.notify(mCurrentWidget);
		}

		if (hasOpen)
		{
			for (auto* child : children)
			{
				if (child != nullptr)
				{
					drawWidget(child, false);
				}
			}

			ImGui::TreePop();
		}
	}
}