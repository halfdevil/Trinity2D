#pragma once

#include "Widgets/Viewport.h"

namespace Trinity
{
	class Sprite;
	class BatchRenderer;

	class SpriteViewport : public Viewport
	{
	public:

		static constexpr const char* kTexturedShader = "/Assets/Engine/Shaders/Textured.wgsl";
		static constexpr const char* kColoredShader = "/Assets/Engine/Shaders/Colored.wgsl";

		SpriteViewport() = default;
		virtual ~SpriteViewport() = default;

		SpriteViewport(const SpriteViewport&) = delete;
		SpriteViewport& operator = (const SpriteViewport&) = delete;

		SpriteViewport(SpriteViewport&&) = default;
		SpriteViewport& operator = (SpriteViewport&&) = default;

		virtual bool create(EditorResources& resources);
		virtual void destroy() override;

		virtual void setSprite(Sprite& sprite);
		virtual void drawContent(float deltaTime) override;

	protected:

		virtual void drawSprite(Sprite* sprite);
		virtual void onViewportResize(uint32_t width, uint32_t height) override;

	protected:

		Sprite* mSprite{ nullptr };
		glm::vec2 mPadding{ 10.0f };
		std::unique_ptr<BatchRenderer> mRenderer{ nullptr };
	};
}