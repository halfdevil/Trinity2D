#pragma once

#include <string>
#include <type_traits>
#include <vector>
#include "imgui.h"
#include "glm/glm.hpp"

namespace Trinity
{
	enum class FileType
	{
		Texture
	};

	enum class ListBoxOperation
	{
		None,
		Add,
		Delete,
		Up,
		Down
	};

	class IEditorLayout
	{
	public:

		virtual bool beginLayout(const std::string& label) const = 0;
		virtual void endLayout() const = 0;

		virtual bool inputInt8(const std::string& label, int8_t& value, int8_t step = 1, int8_t stepFast = 100) const = 0;
		virtual bool inputInt16(const std::string& label, int16_t& value, int16_t step = 1, int16_t stepFast = 100) const = 0;
		virtual bool inputInt32(const std::string& label, int32_t& value, int32_t step = 1, int32_t stepFast = 100) const = 0;
		virtual bool inputInt64(const std::string& label, int64_t& value, int64_t step = 1, int64_t stepFast = 100) const = 0;
		virtual bool inputUInt8(const std::string& label, uint8_t& value, uint8_t step = 1, uint8_t stepFast = 100) const = 0;
		virtual bool inputUInt16(const std::string& label, uint16_t& value, uint16_t step = 1, uint16_t stepFast = 100) const = 0;
		virtual bool inputUInt32(const std::string& label, uint32_t& value, uint32_t step = 1, uint32_t stepFast = 100) const = 0;
		virtual bool inputUInt64(const std::string& label, uint64_t& value, uint64_t step = 1, uint64_t stepFast = 100) const = 0;
		virtual bool inputFloat(const std::string& label, float& value, float step = 1.0f, float stepFast = 100.0f) const = 0;
		virtual bool inputDouble(const std::string& label, double& value, double step = 1.0, double stepFast = 100.0) const = 0;

		virtual bool inputVec2(const std::string& label, glm::vec2& value) const = 0;
		virtual bool inputVec3(const std::string& label, glm::vec3& value) const = 0;
		virtual bool inputVec4(const std::string& label, glm::vec4& value) const = 0;
		virtual bool inputQuat(const std::string& label, glm::quat& value) const = 0;
		virtual bool inputString(const std::string& label, std::string& value) const = 0;
		virtual bool inputSize(const std::string& label, glm::uvec2& value) const = 0;
		virtual bool checkbox(const std::string& label, bool& value) const = 0;

		virtual bool beginCombo(const std::string& label, const std::string& preview) const = 0;
		virtual bool comboItem(const std::string& label, bool selected) const = 0;
		virtual void endCombo() const = 0;

		virtual bool fileCombo(const std::string& label, FileType fileType, std::string& selectedFile) const = 0;
		virtual ListBoxOperation listBox(const std::string& label, int32_t& selectedIndex, const std::vector<const char*>& items) const = 0;

		virtual bool beginListBox(const std::string& label) const = 0;
		virtual bool listItem(const std::string& label, bool selected) const = 0;
		virtual void endListBox() const = 0;
		virtual ListBoxOperation listBoxEditor() const = 0;
	};
}