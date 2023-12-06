#pragma once

#include "Editor/EditorLayout.h"
#include "VFS/Storage.h"
#include "imgui.h"
#include "glm/glm.hpp"
#include <string>
#include <type_traits>
#include <unordered_map>

namespace Trinity
{
	class EditorLayout : public IEditorLayout
	{
	public:

		static constexpr const uint32_t kMaxIdSize = 512;

		EditorLayout() = default;
		virtual ~EditorLayout() = default;

		EditorLayout(const EditorLayout&) = delete;
		EditorLayout& operator = (const EditorLayout&) = delete;

		EditorLayout(EditorLayout&&) = default;
		EditorLayout& operator = (EditorLayout&&) = default;

		virtual bool beginLayout(const std::string& label) const override;
		virtual void endLayout() const override;

		virtual bool inputInt8(const std::string& label, int8_t& value, int8_t step = 1, int8_t stepFast = 100) const override;
		virtual bool inputInt16(const std::string& label, int16_t& value, int16_t step = 1, int16_t stepFast = 100) const override;
		virtual bool inputInt32(const std::string& label, int32_t& value, int32_t step = 1, int32_t stepFast = 100) const override;
		virtual bool inputInt64(const std::string& label, int64_t& value, int64_t step = 1, int64_t stepFast = 100) const override;
		virtual bool inputUInt8(const std::string& label, uint8_t& value, uint8_t step = 1, uint8_t stepFast = 100) const override;
		virtual bool inputUInt16(const std::string& label, uint16_t& value, uint16_t step = 1, uint16_t stepFast = 100) const override;
		virtual bool inputUInt32(const std::string& label, uint32_t& value, uint32_t step = 1, uint32_t stepFast = 100) const override;
		virtual bool inputUInt64(const std::string& label, uint64_t& value, uint64_t step = 1, uint64_t stepFast = 100) const override;
		virtual bool inputFloat(const std::string& label, float& value, float step = 1.0f, float stepFast = 100.0f) const override;
		virtual bool inputDouble(const std::string& label, double& value, double step = 1.0, double stepFast = 100.0) const override;

		virtual bool inputVec2(const std::string& label, glm::vec2& value) const override;
		virtual bool inputVec3(const std::string& label, glm::vec3& value) const override;
		virtual bool inputVec4(const std::string& label, glm::vec4& value) const override;
		virtual bool inputQuat(const std::string& label, glm::quat& value) const override;
		virtual bool inputString(const std::string& label, std::string& value) const override;
		virtual bool checkbox(const std::string& label, bool& value) const override;

		virtual bool beginCombo(const std::string& label, const std::string& preview) const override;
		virtual bool comboItem(const std::string& label, bool selected) const override;
		virtual void endCombo() const;

		virtual bool fileCombo(const std::string& label, FileType fileType, std::string& selectedFile) const override;
		virtual ListBoxOperation listBox(const std::string& label, int32_t& selectedIndex, const std::vector<const char*>& items) const;

		virtual bool updateFiles(const std::string& dir, FileType fileType);

	public:

		template <typename T>
		ImGuiDataType getDataType() const
		{
			if constexpr (std::is_same<T, int8_t>::value)			return ImGuiDataType_S8;
			else if constexpr (std::is_same<T, int16_t>::value)		return ImGuiDataType_S16;
			else if constexpr (std::is_same<T, int32_t>::value)		return ImGuiDataType_S32;
			else if constexpr (std::is_same<T, int64_t>::value)		return ImGuiDataType_S64;
			else if constexpr (std::is_same<T, uint8_t>::value)		return ImGuiDataType_U8;
			else if constexpr (std::is_same<T, uint16_t>::value)	return ImGuiDataType_U16;
			else if constexpr (std::is_same<T, uint32_t>::value)	return ImGuiDataType_U32;
			else if constexpr (std::is_same<T, uint64_t>::value)	return ImGuiDataType_U64;
			else if constexpr (std::is_same<T, float>::value)		return ImGuiDataType_Float;
			else if constexpr (std::is_same<T, double>::value)		return ImGuiDataType_Double;

			return ImGuiDataType_U32;
		}

		template <typename T>
		bool inputScalar(const std::string& label, T& value, T step = T(1), T stepFast = T(100)) const
		{
			auto type = getDataType<T>();
			const char* format = "%d";

			switch (type)
			{
			case ImGuiDataType_Float:
				format = "%.3f";
				break;

			case ImGuiDataType_Double:
				format = "%.3lf";
				break;

			default:
				break;
			}

			char id[kMaxIdSize];
			std::snprintf(id, kMaxIdSize, "##%s", label.c_str());

			ImGui::PushID(id);
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(label.c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-FLT_MIN);

			bool ret = ImGui::InputScalar("##value", type, &value, &step, &stepFast, format);
			ImGui::PopID();

			return ret;
		}

		template <typename T>
		bool sliderScalar(const std::string& label, T& value, T min, T max) const
		{
			auto type = getDataType<T>();
			const char* format = "%d";

			switch (type)
			{
			case ImGuiDataType_Float:
				format = "%.3f";
				break;

			case ImGuiDataType_Double:
				format = "%.3lf";
				break;

			default:
				break;
			}

			char id[kMaxIdSize];
			std::snprintf(id, kMaxIdSize, "##%s", label.c_str());

			ImGui::PushID(id);
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(label.c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-FLT_MIN);

			bool ret = ImGui::SliderScalar("##value", type, &value, &min, &max, format);
			ImGui::PopID();

			return ret;
		}

	protected:

		std::unordered_map<FileType, std::vector<FileEntry>> mFiles;
	};
}