#pragma once

#include <string>
#include <type_traits>
#include "imgui.h"
#include "glm/glm.hpp"

namespace Trinity
{
	class EditorLayout
	{
	public:

		static constexpr const uint32_t kMaxIdSize = 512;

		bool beginLayout(const std::string& label) const;
		void endLayout() const;

		bool inputVec2(const std::string& label, glm::vec2& value) const;
		bool inputVec3(const std::string& label, glm::vec3& value) const;
		bool inputVec4(const std::string& label, glm::vec4& value) const;
		bool inputQuat(const std::string& label, glm::quat& value) const;

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
	};
}