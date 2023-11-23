#include "Widgets/ContextMenu.h"

namespace Trinity
{
	void ContextMenu::draw()
	{
		if (ImGui::BeginPopupContextItem(mTitle.c_str(), ImGuiPopupFlags_MouseButtonRight))
		{
			mIsOpened = true;

			Menu::draw();
			ImGui::EndPopup();
		}
		else
		{
			mIsOpened = false;
		}
	}
}