#include "Widgets/SelectionWindow.h"
#include "Core/EditorTheme.h"

namespace Trinity
{
	void SelectionWindow::setItems(std::vector<SelectionItem>&& items)
	{
		mItems = std::move(items);

		for (const auto& item : mItems)
		{
			mItemNames.push_back(item.title.c_str());
		}
	}

	void SelectionWindow::setButtonTitle(const std::string& buttonTitle)
	{
		mButtonTitle = buttonTitle;
	}

	void SelectionWindow::draw()
	{
		if (!isEnabled())
		{
			return;
		}

		auto& theme = EditorTheme::get();

		ImGui::Begin(mTitle.c_str(), &mEnabled);
		{
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::ListBox("##selection", (int*)&mSelectedIndex, mItemNames.data(), (int)mItemNames.size());
			ImGui::Separator();

			auto currentPos = ImGui::GetCursorPos();
			auto buttonPos = ImVec2{ currentPos.x + (ImGui::GetContentRegionAvail().x - 
				mButtonWidth) / 2.0f, currentPos.y };

			ImGui::SetCursorPos(buttonPos);
			if (ImGui::Button(mButtonTitle.c_str(), ImVec2{ mButtonWidth, theme.buttonSize.y }))
			{
				onSelect.notify(mItems[mSelectedIndex]);
			}

			mButtonWidth = ImGui::GetItemRectSize().x;
			ImGui::End();
		}
	}
}