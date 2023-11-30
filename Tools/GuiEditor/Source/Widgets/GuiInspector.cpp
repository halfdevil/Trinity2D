#include "Widgets/GuiInspector.h"
#include "Gui/Gui.h"
#include "Gui/Widget.h"
#include "Gui/WidgetFactory.h"
#include "Editor/Editor.h"
#include "Core/EditorLayout.h"
#include "Core/EditorTheme.h"
#include "misc/cpp/imgui_stdlib.h"

namespace Trinity
{
	void GuiInspector::setGui(Gui& gui)
	{
		mGui = &gui;
		setSelectedWidget(mGui->getRoot());
	}

	void GuiInspector::setSelectedWidget(Widget* widget)
	{
		mSelectedWidget = widget;
		if (mSelectedWidget != nullptr)
		{
			mSelectedWidgetName = mSelectedWidget->getName();
		}
	}

	void GuiInspector::drawInspector()
	{
		if (mSelectedWidget != nullptr)
		{
			ImGui::TextUnformatted("Name: ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

			if (ImGui::InputText("##name", &mSelectedWidgetName))
			{
				mSelectedWidget->setName(mSelectedWidgetName);
			}

			ImGui::Separator();

			if (auto* editor = mSelectedWidget->getEditor(*mGui); editor != nullptr)
			{
				editor->onInspectorGui(*mLayout, *mResourceCache);
			}
		}
	}
}