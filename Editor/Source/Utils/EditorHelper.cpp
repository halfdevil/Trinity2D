#include "Utils/EditorHelper.h"
#include "Graphics/Texture.h"
#include "Core/Logger.h"
#include "imgui_internal.h"
#include "IconsFontAwesome6.h"

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

	bool EditorHelper::imageButton(ImGuiID id, Texture* texture, const glm::vec2& size)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });

		ImVec2 position = ImGui::GetCursorPos();
		ImVec2 imgSize = { (float)texture->getWidth(), (float)texture->getHeight() };
		ImVec2 imgPosition = { position.x + size.x / 2 - imgSize.x / 2, position.y + size.y / 2 - imgSize.y / 2 };

		ImGui::Button("##imageButton", { size.x, size.y });		
		bool clicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);	
		ImGui::PopStyleColor();

		ImGui::SetCursorPos(imgPosition);
		ImGui::Image(texture, imgSize);

		return clicked;
	}

	bool EditorHelper::assetIcon(ImGuiID id, Texture* texture, const glm::vec2& iconSize, const std::string& text, 
		const glm::vec2& textSize, float textPadding, bool& doubleClicked)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);

		ImVec2 position = ImGui::GetCursorPos();
		ImVec2 buttonSize = { iconSize.x, iconSize.y + textSize.y };
		ImVec2 imgSize = { (float)texture->getWidth(), (float)texture->getHeight() };
		ImVec2 imgPosition = { position.x + (iconSize.x - imgSize.x) / 2, position.y + (iconSize.y - imgSize.y) / 2 };

		ImGui::Button("##imageButton", buttonSize);

		bool clicked = ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
		doubleClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

		ImGui::SetCursorPos(imgPosition);
		ImGui::Image(texture, imgSize);

		ImVec2 newTextSize = ImGui::CalcTextSize(text.c_str());
		if (newTextSize.x < textSize.x - textPadding)
		{
			ImVec2 textPosition = { position.x + (textSize.x - newTextSize.x) / 2, position.y + iconSize.y };
			ImGui::SetCursorPos(textPosition);
			ImGui::TextUnformatted(text.c_str());
		}
		else
		{
			ImVec2 textPosition = { position.x + textPadding, position.y + iconSize.y };
			ImGui::SetCursorPos(textPosition);
			ImGui::PushTextWrapPos(position.x + textSize.x - textPadding);
			ImGui::TextUnformatted(text.c_str());
			ImGui::PopTextWrapPos();
		}

		ImGui::PopStyleVar();
		return clicked;
	}

	ListBoxOperation EditorHelper::listBoxEditor(const char* id)
	{
		ListBoxOperation operation{ ListBoxOperation::None };

		ImGui::PushID(id);

		if (ImGui::Button(ICON_FA_PLUS))
		{
			operation = ListBoxOperation::Add;
		}

		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_TRASH))
		{
			operation = ListBoxOperation::Delete;
		}

		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_CARET_UP))
		{
			operation = ListBoxOperation::Up;
		}

		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_CARET_DOWN))
		{
			operation = ListBoxOperation::Down;
		}

		ImGui::PopID();
		return operation;
	}
}