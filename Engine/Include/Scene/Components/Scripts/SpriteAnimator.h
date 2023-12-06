#pragma once

#include "Scene/Components/Script.h"

namespace Trinity
{
	struct SpriteAnimation;
	class SpriteRenderable;

	enum class SpriteAnimationType
	{
		Left,
		Right,
		Swing
	};

	enum class SpriteAnimationState
	{
		Playing,
		Paused,
		Stopped
	};

	class SpriteAnimator : public Script
	{
	public:

		SpriteAnimator() = default;
		virtual ~SpriteAnimator() = default;

		SpriteAnimator(const SpriteAnimator&) = delete;
		SpriteAnimator& operator = (const SpriteAnimator&) = delete;

		SpriteAnimator(SpriteAnimator&&) = default;
		SpriteAnimator& operator = (SpriteAnimator&&) = default;

		SpriteAnimationType getAnimationType() const
		{
			return mAnimationType;
		}

		bool isLooping() const
		{
			return mLooping;
		}

		float getSpeed() const
		{
			return mSpeed;
		}

		uint32_t getCurrentFrame() const
		{
			return mCurrentFrame;
		}

		SpriteAnimationState getState() const
		{
			return mState;
		}

		virtual bool play(const std::string& name, SpriteAnimationType animationType = SpriteAnimationType::Left, 
			bool looping = false, float frameLength = 100.0f);

		virtual void setSpeed(float frameLength);
		virtual void setLooping(bool looping);
		virtual void pause(bool paused);
		virtual void stop();

		virtual bool init() override;
		virtual void update(float deltaTime) override;

		virtual UUIDv4::UUID getTypeUUID() const override;

	public:

		inline static UUIDv4::UUID UUID = UUIDv4::UUID::fromStrFactory("fe1f609a-50a5-4adb-95ee-75c38114fe32");

	protected:

		SpriteAnimationType mAnimationType{ SpriteAnimationType::Left };
		SpriteRenderable* mRenderable{ nullptr };
		SpriteAnimation* mCurrentAnimation{ nullptr };
		uint32_t mCurrentFrame{ 0 };
		float mCurrentFrameTime{ 0.0f };
		float mSpeed{ 100.0f };
		bool mLooping{ true };
		bool mForwardAnimation{ true };
		SpriteAnimationState mState{ SpriteAnimationState::Stopped };
	};
}