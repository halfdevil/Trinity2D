#include "Utils/EditorHelper.h"
#include "imgui_internal.h"

namespace Trinity
{
	bool EditorHelper::toggleButton(const std::string& label, bool state, const glm::vec2& size)
	{
		if (state)
		{
			auto color = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
			ImGui::PushStyleColor(ImGuiCol_Button, color);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
		}

		bool ret = ImGui::ButtonEx(label.c_str(), { size.x, size.y });
		if (state)
		{
			ImGui::PopStyleColor(3);
		}

		return ret;
	}
}