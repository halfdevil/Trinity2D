#include "Widgets/MenuBar.h"

namespace Trinity
{
	void MenuBar::draw()
	{
		if (ImGui::BeginMainMenuBar())
		{
			Menu::draw();
			ImGui::EndMainMenuBar();
		}
	}
}