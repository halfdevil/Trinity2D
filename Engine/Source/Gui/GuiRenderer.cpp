#include "Gui/GuiRenderer.h"
#include "Gui/Font.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderTarget.h"
#include "Core/Logger.h"
#include "Core/ResourceCache.h"

namespace Trinity
{
	bool GuiRenderer::drawText(
		const std::string& text, 
		Font* font, 
		float fontSize, 
		const glm::vec2& position, 
		const glm::vec2& origin, 
		const glm::mat4& transform, 
		const glm::vec4& color)
	{
		if (!font->hasSize(fontSize))
		{
			LogError("Font not loaded with size: %f", fontSize);
			return false;
		}

		auto* texture = font->getTexture();
		auto* metrics = font->getMetrics(fontSize);
		auto& glyphs = metrics->glyphs;
		auto offset = metrics->lineDistance - metrics->lineGap;

		auto fontWidth = texture->getWidth();
		auto fontHeight = texture->getHeight();

		glm::vec2 textSize{ 0.0f };
		glm::vec3 textPosition{ position, 0.0f };

		for (auto& c : text)
		{
			auto code = c - 32;
			auto& charInfo = glyphs[code];

			textSize.x += charInfo.xadvance;
			auto height = (charInfo.yoff2 - charInfo.yoff) / fontHeight;

			if (textSize.y < height)
			{
				textSize.y = height;
			}
		}

		glm::vec3 localOrigin {
			textSize.x * origin.x,
			textSize.y * origin.y,
			0.0f
		};

		for (auto& c : text)
		{
			auto code = c - 32;
			auto& charInfo = glyphs[code];

			float x1 = textPosition.x + charInfo.xoff - localOrigin.x;
			float y1 = textPosition.y + charInfo.yoff + offset - localOrigin.y;
			float x2 = textPosition.x + charInfo.xoff2 - localOrigin.x;
			float y2 = textPosition.y + charInfo.yoff2 + offset - localOrigin.y;

			glm::vec4 p1 = transform * glm::vec4{ x1, y1, 0.0f, 1.0f };
			glm::vec4 p2 = transform * glm::vec4{ x1, y2, 0.0f, 1.0f };
			glm::vec4 p3 = transform * glm::vec4{ x2, y2, 0.0f, 1.0f };
			glm::vec4 p4 = transform * glm::vec4{ x2, y1, 0.0f, 1.0f };

			float u1 = charInfo.x0 / (float)fontWidth;
			float v1 = charInfo.y0 / (float)fontHeight;
			float u2 = charInfo.x1 / (float)fontWidth;
			float v2 = charInfo.y1 / (float)fontHeight;

			Vertex vertices[4] = {
				{ .position = glm::vec3(p1), .uv = { u1, v2 }, .color = color },
				{ .position = glm::vec3(p2), .uv = { u1, v1 }, .color = color },
				{ .position = glm::vec3(p3), .uv = { u2, v1 }, .color = color },
				{ .position = glm::vec3(p4), .uv = { u2, v2 }, .color = color }
			};

			auto numVertices = mStagingContext.numVertices;
			auto baseIndex = mStagingContext.numIndices;

			uint32_t indices[6] = {
				numVertices,
				numVertices + 1,
				numVertices + 2,
				numVertices + 2,
				numVertices + 3,
				numVertices
			};

			addVertices(vertices, 4);
			addIndices(indices, 6);

			if (!addCommand(texture, baseIndex, 6))
			{
				LogError("BatchRenderer::addCommand() failed");
				return false;
			}

			textPosition.x += charInfo.xadvance;
		}

		return true;
	}
}