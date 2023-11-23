#pragma once

#include <string>
#include "imgui.h"
#include "glm/glm.hpp"

namespace Trinity
{
	class Texture;

	class EditorHelper
	{
	public:

		static bool toggleButton(const std::string& label, bool state,
			const glm::vec2& size = glm::vec2{ 0.0f });

		static bool imageButton(ImGuiID id, Texture* texture, const glm::vec2& size);
		static bool assetIcon(ImGuiID id, Texture* texture, const glm::vec2& iconSize, const std::string& text, 
			const glm::vec2& textSize, float textPadding, bool& doubleClicked);
	};
}