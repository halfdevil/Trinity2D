#include "Core/EditorLayout.h"
#include "Core/Logger.h"
#include "Utils/EditorHelper.h"
#include "VFS/FileSystem.h"
#include "glm/gtc/type_ptr.hpp"
#include "misc/cpp/imgui_stdlib.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

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

	bool EditorLayout::inputInt8(const std::string& label, int8_t& value, int8_t step, int8_t stepFast) const
	{
		return inputScalar(label, value, step, stepFast);
	}

	bool EditorLayout::inputInt16(const std::string& label, int16_t& value, int16_t step, int16_t stepFast) const
	{
		return inputScalar(label, value, step, stepFast);
	}

	bool EditorLayout::inputInt32(const std::string& label, int32_t& value, int32_t step, int32_t stepFast) const
	{
		return inputScalar(label, value, step, stepFast);
	}

	bool EditorLayout::inputInt64(const std::string& label, int64_t& value, int64_t step, int64_t stepFast) const
	{
		return inputScalar(label, value, step, stepFast);
	}

	bool EditorLayout::inputUInt8(const std::string& label, uint8_t& value, uint8_t step, uint8_t stepFast) const
	{
		return inputScalar(label, value, step, stepFast);
	}

	bool EditorLayout::inputUInt16(const std::string& label, uint16_t& value, uint16_t step, uint16_t stepFast) const
	{
		return inputScalar(label, value, step, stepFast);
	}

	bool EditorLayout::inputUInt32(const std::string& label, uint32_t& value, uint32_t step, uint32_t stepFast) const
	{
		return inputScalar(label, value, step, stepFast);
	}

	bool EditorLayout::inputUInt64(const std::string& label, uint64_t& value, uint64_t step, uint64_t stepFast) const
	{
		return inputScalar(label, value, step, stepFast);
	}

	bool EditorLayout::inputFloat(const std::string& label, float& value, float step, float stepFast) const
	{
		return inputScalar(label, value, step, stepFast);
	}

	bool EditorLayout::inputDouble(const std::string& label, double& value, double step, double stepFast) const
	{
		return inputScalar(label, value, step, stepFast);
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

	bool EditorLayout::inputString(const std::string& label, std::string& value) const
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

		bool ret = ImGui::InputText("##value", &value);
		ImGui::PopID();

		return ret;
	}

	bool EditorLayout::checkbox(const std::string& label, bool& value) const
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

		bool ret = ImGui::Checkbox("##value", &value);
		ImGui::PopID();

		return ret;
	}

	bool EditorLayout::beginCombo(const std::string& label, const std::string& preview) const
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

		bool ret = ImGui::BeginCombo("##combo", preview.c_str());
		if (!ret)
		{
			ImGui::PopID();
		}

		return ret;
	}

	bool EditorLayout::comboItem(const std::string& label, bool selected) const
	{
		bool ret = ImGui::Selectable(label.c_str(), selected);
		if (ret)
		{
			ImGui::SetItemDefaultFocus();
		}

		return ret;
	}

	void EditorLayout::endCombo() const
	{
		ImGui::EndCombo();
		ImGui::PopID();
	}

	bool EditorLayout::fileCombo(const std::string& label, FileType fileType, std::string& selectedFile) const
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

		bool ret = false;

		if (auto it = mFiles.find(fileType); it != mFiles.end())
		{
			auto& fileSystem = FileSystem::get();
			auto fileName = fileSystem.getFileName(selectedFile);

			if (ImGui::BeginCombo("##combo", fileName.c_str()))
			{
				auto& entries = it->second;
				for (auto& entry : entries)
				{
					bool selected = entry.path == selectedFile;
					if (ImGui::Selectable(entry.name.c_str(), selected))
					{
						selectedFile = entry.path;
						ret = true;
					}

					if (selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
		}

		ImGui::PopID();
		return ret;
	}

	ListBoxOperation EditorLayout::listBox(const std::string& label, int32_t& selectedIndex, const std::vector<const char*>& items) const
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

		auto ret = EditorHelper::listBox("##listBox", selectedIndex, items);
		ImGui::PopID();

		return ret;
	}

	bool EditorLayout::updateFiles(const std::string& dir, FileType fileType)
	{
		auto& fileSystem = FileSystem::get();
		
		if (fileType == FileType::Texture)
		{
			std::vector<FileEntry> fileEntries;
			std::vector<std::string> extensions = { ".png", ".jpg" };

			if (!fileSystem.getFiles(dir, true, extensions, fileEntries))
			{
				LogError("FileSystem::getFiles() failed for: '%s'", dir.c_str());
				return false;
			}

			if (auto it = mFiles.find(fileType); it != mFiles.end())
			{
				it->second = std::move(fileEntries);
			}
			else
			{
				mFiles.insert(std::make_pair(fileType, std::move(fileEntries)));
			}
		}

		return true;
	}
}