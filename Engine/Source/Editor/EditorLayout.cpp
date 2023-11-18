#include "Editor/EditorLayout.h"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	bool EditorLayout::beginLayout(const std::string& label) const
	{
		if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			char id[kMaxIdSize];
			std::snprintf(id, kMaxIdSize, "##%s", label.c_str());

			return ImGui::BeginTable(id, 2, ImGuiTableFlags_Resizable);
		}

		return false;
	}
	
	void EditorLayout::endLayout() const
	{
		ImGui::EndTable();
	}

	bool EditorLayout::inputVec2(const std::string& label, glm::vec2& value) const
	{
		char id[kMaxIdSize];
		std::snprintf(id, kMaxIdSize, "##%s", label.c_str());

		ImGui::PushID(id);
		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		bool ret = ImGui::InputFloat2("##value", glm::value_ptr(value));
		ImGui::PopID();

		return ret;
	}

	bool EditorLayout::inputVec3(const std::string& label, glm::vec3& value) const
	{
		char id[kMaxIdSize];
		std::snprintf(id, kMaxIdSize, "##%s", label.c_str());

		ImGui::PushID(id);
		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
		
		bool ret = ImGui::InputFloat3("##value", glm::value_ptr(value));
		ImGui::PopID();

		return ret;
	}

	bool EditorLayout::inputVec4(const std::string& label, glm::vec4& value) const
	{
		char id[kMaxIdSize];
		std::snprintf(id, kMaxIdSize, "##%s", label.c_str());

		ImGui::PushID(id);
		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
		
		bool ret = ImGui::InputFloat4("##value", glm::value_ptr(value));
		ImGui::PopID();

		return ret;
	}

	bool EditorLayout::inputQuat(const std::string& label, glm::quat& value) const
	{
		char id[kMaxIdSize];
		std::snprintf(id, kMaxIdSize, "##%s", label.c_str());

		ImGui::PushID(id);
		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
		
		bool ret = ImGui::InputFloat4("##value", glm::value_ptr(value));
		ImGui::PopID();

		return ret;
	}
}