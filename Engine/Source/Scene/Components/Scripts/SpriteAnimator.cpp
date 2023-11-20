#include "Scene/Components/Scripts/SpriteAnimator.h"
#include "Scene/Components/SpriteRenderable.h"
#include "Scene/Node.h"
#include "Scene/Sprite.h"
#include "Core/Logger.h"

namespace Trinity
{
	bool SpriteAnimator::play(const std::string& name, SpriteAnimationType animationType, 
		bool looping, float frameLength)
	{
		auto* sprite = mRenderable->getSprite();
		auto* animation = sprite->getAnimation(name);

		if (animation == nullptr)
		{
			LogError("Sprite animation '%s' not found", name.c_str());
			return false;
		}

		mState = PlayState::Playing;
		mCurrentAnimation = animation;
		mCurrentFrame = 0;
		mCurrentFrameTime = 0.0f;
		mAnimationType = animationType;

		return true;
	}

	void SpriteAnimator::pause(bool paused)
	{
		mState = PlayState::Paused;
	}

	void SpriteAnimator::stop()
	{
		mState = PlayState::Stopped;
		mCurrentFrame = 0;
		mCurrentFrameTime = 0;
	}

	bool SpriteAnimator::init()
	{
		if (!Script::init())
		{
			return false;
		}

		if (!mNode->hasComponent<SpriteRenderable>())
		{
			LogError("SpriteRenderable not attached with the node");
			return false;
		}

		mRenderable = &mNode->getComponent<SpriteRenderable>();
		return true;
	}

	void SpriteAnimator::update(float deltaTime)
	{
		if (mState != PlayState::Playing)
		{
			return;
		}

		mCurrentFrameTime += deltaTime * (1.0f / mFrameLength);
		mCurrentFrame = (uint32_t)std::floor(mCurrentFrameTime);

		auto& frames = mCurrentAnimation->frames;
		uint32_t numFrames = (uint32_t)frames.size();

		if (mCurrentFrame < numFrames)
		{
			switch (mAnimationType)
			{
			case SpriteAnimationType::Right:
				mCurrentFrame = numFrames - mCurrentFrame - 1;
				break;

			case SpriteAnimationType::Swing:
				if (!mForwardAnimation)
				{
					mCurrentFrame = numFrames - mCurrentFrame - 1;
				}
				break;

			default:
				break;
			}
		}
		else
		{
			if (mLooping)
			{
				switch (mAnimationType)
				{
				case SpriteAnimationType::Left:
					mCurrentFrame = 0;
					break;

				case SpriteAnimationType::Right:
					mCurrentFrame = numFrames - 1;
					break;

				case SpriteAnimationType::Swing:
					if (mForwardAnimation)
					{
						mCurrentFrame = numFrames - 1;
					}
					else
					{
						mCurrentFrame = 0;
					}

					mForwardAnimation = !mForwardAnimation;
					break;

				default:
					break;
				}
			}
			else
			{
				switch (mAnimationType)
				{
				case SpriteAnimationType::Left:
					mCurrentFrame = numFrames - 1;
					break;

				case SpriteAnimationType::Right:
					mCurrentFrame = 0;
					break;

				case SpriteAnimationType::Swing:
					if (mForwardAnimation)
					{
						mCurrentFrame = numFrames - 1;
					}
					else
					{
						mCurrentFrame = 0;
					}
					break;

				default:
					break;
				}
			}

			mCurrentFrameTime = 0.0f;
		}
	}

	UUIDv4::UUID SpriteAnimator::getUUID() const
	{
		return SpriteAnimator::UUID;
	}
}