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

	class SpriteAnimator : public Script
	{
	public:

		enum class PlayState
		{
			Playing,
			Paused,
			Stopped
		};

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

		float getFrameLength() const
		{
			return mFrameLength;
		}

		virtual bool play(const std::string& name, SpriteAnimationType animationType = SpriteAnimationType::Left, 
			bool looping = false, float frameLength = 100.0f);

		virtual void pause(bool paused);
		virtual void stop();

		virtual bool init() override;
		virtual void update(float deltaTime) override;

		virtual UUIDv4::UUID getUUID() const override;

	public:

		inline static UUIDv4::UUID UUID = UUIDv4::UUID::fromStrFactory("fe1f609a-50a5-4adb-95ee-75c38114fe32");

	protected:

		SpriteAnimationType mAnimationType{ SpriteAnimationType::Left };
		SpriteRenderable* mRenderable{ nullptr };
		SpriteAnimation* mCurrentAnimation{ nullptr };
		uint32_t mCurrentFrame{ 0 };
		float mCurrentFrameTime{ 0.0f };
		float mFrameLength{ 0.0f };
		bool mLooping{ false };
		bool mForwardAnimation{ true };
		PlayState mState{ PlayState::Stopped };
	};
}