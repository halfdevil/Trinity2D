#pragma once

#include <string>
#include "imgui.h"
#include "glm/glm.hpp"

namespace Trinity
{
	class EditorHelper
	{
	public:

		static bool toggleButton(const std::string& label, bool state,
			const glm::vec2& size = glm::vec2{ 0.0f });
	};
}