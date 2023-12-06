#pragma once

#include "Widgets/AnimationPlayer.h"

namespace Trinity
{
	class Sprite;
	class SpriteRenderable;
	class SpriteAnimator;
	struct SpriteAnimation;

	class SpriteAnimationPlayer : public AnimationPlayer
	{
	public:

		static constexpr float kBaseAnimationSpeed = 10.0f;

		SpriteAnimationPlayer() = default;
		virtual ~SpriteAnimationPlayer() = default;

		SpriteAnimationPlayer(const SpriteAnimationPlayer&) = delete;
		SpriteAnimationPlayer& operator = (const SpriteAnimationPlayer&) = delete;

		SpriteAnimationPlayer(SpriteAnimationPlayer&&) = default;
		SpriteAnimationPlayer& operator = (SpriteAnimationPlayer&&) = default;

		virtual bool create(EditorResources& resources) override;
		virtual void destroy() override;

		virtual void setSprite(Sprite& sprite);
		virtual void setFrameLength(float frameLength);
		virtual void setAnimation(const std::string& animationName);
		virtual void update(float deltaTime) override;

	protected:

		virtual Scene* createSpriteScene(ResourceCache& cache);
		virtual void onViewportResize(uint32_t width, uint32_t height) override;

		virtual void onPlayPauseClick(bool playing) override;
		virtual void onStopClick() override;
		virtual void onSpeedChange(float speed) override;
		virtual void onLoopClick(bool loop) override;

	protected:

		Sprite* mSprite{ nullptr };
		std::string mAnimationName;
		SpriteRenderable* mRenderable{ nullptr };
		SpriteAnimator* mAnimator{ nullptr };
		float mBaseSpeed{ 10.0f };
		bool mLoop{ true };
	};
}