#pragma once

#include "Graphics/BatchRenderer.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "webgpu/webgpu_cpp.h"

namespace Trinity
{
	class Font;

	class GuiRenderer : public BatchRenderer
	{
	public:

		GuiRenderer() = default;
		virtual ~GuiRenderer() = default;

		GuiRenderer(const GuiRenderer&) = delete;
		GuiRenderer& operator = (const GuiRenderer&) = delete;

		GuiRenderer(GuiRenderer&&) = default;
		GuiRenderer& operator = (GuiRenderer&&) = default;

		virtual bool drawText(
			const std::string& text, 
			Font* font, 
			float fontSize, 
			const glm::vec2& position, 
			const glm::vec2& origin, 
			const glm::mat4& transform,
			const glm::vec4& color
		);
	};
}