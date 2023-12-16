#pragma once

#include "Widgets/Viewport.h"

namespace Trinity
{
	class Sprite;

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
		virtual void setSelectedFrame(uint32_t selectedFrame);
		virtual void drawContent(float deltaTime) override;

	protected:

		virtual void drawSprite();
		virtual void onViewportResize(uint32_t width, uint32_t height) override;
		virtual glm::vec2 convertToViewport(const glm::vec2& v) const override;

	public:

		Observer<uint32_t> onSelectFrame;

	protected:

		Sprite* mSprite{ nullptr };
		glm::vec2 mPadding{ 10.0f };
		uint32_t mSelectedFrame{ 0 };
	};
}