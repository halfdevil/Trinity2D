#include "Widgets/SpriteAnimationPlayer.h"
#include "Scene/Scene.h"
#include "Scene/SceneSystem.h"
#include "Scene/Components/SpriteRenderable.h"
#include "Scene/Components/Scripts/SpriteAnimator.h"
#include "Core/EditorResources.h"
#include "Core/EditorCamera.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"

namespace Trinity
{
	bool SpriteAnimationPlayer::create(EditorResources& resources)
	{
		if (!AnimationPlayer::create(resources))
		{
			LogError("AnimationPlayer::create() failed");
			return false;
		}

		auto* scene = createSpriteScene(*resources.getResourceCache());
		if (!scene)
		{
			LogError("SpriteAnimationPlayer::createSpriteScene() failed");
			return false;
		}

		setScene(*scene);

		mRenderable = scene->getComponent<SpriteRenderable>();
		mAnimator = scene->getComponent<SpriteAnimator>();

		return true;
	}

	void SpriteAnimationPlayer::destroy()
	{
		AnimationPlayer::destroy();
		mSprite = nullptr;
	}

	void SpriteAnimationPlayer::setSprite(Sprite& sprite)
	{
		mSprite = &sprite;

		if (mRenderable != nullptr)
		{
			mRenderable->setSprite(sprite);
		}
		else
		{
			mRenderable = mScene->addSpriteRenderable(sprite, "Sprite");
			mAnimator = mScene->addSpriteAnimator("Sprite");

			if (!mAnimator->init())
			{
				LogError("SpriteAnimator::init() failed");
			}
		}
	}

	void SpriteAnimationPlayer::setFrameLength(float frameLength)
	{
		mBaseSpeed = frameLength;
	}

	void SpriteAnimationPlayer::setAnimation(const std::string& animationName)
	{
		mAnimationName = animationName;
	}

	void SpriteAnimationPlayer::update(float deltaTime)
	{
		AnimationPlayer::update(deltaTime);

		if (mAnimator != nullptr)
		{
			mAnimator->update(deltaTime);
			mRenderable->setActiveFrameIndex(mAnimator->getCurrentFrame());
		}
	}

	Scene* SpriteAnimationPlayer::createSpriteScene(ResourceCache& cache)
	{
		auto scene = std::make_unique<Scene>();
		scene->setName("New Scene");

		auto root = std::make_unique<Node>();
		root->setName("Root");
		scene->setRoot(*root);
		scene->addNode(std::move(root));

		auto* spriteNode = scene->addEmpty();
		spriteNode->setName("Sprite");
				
		auto* scenePtr = scene.get();
		cache.addResource(std::move(scene));

		return scenePtr;
	}

	void SpriteAnimationPlayer::onViewportResize(uint32_t width, uint32_t height)
	{
		AnimationPlayer::onViewportResize(width, height);

		if (mCamera != nullptr)
		{
			float halfWidth = 0.5f * width;
			float halfHeight = 0.5f * height;

			mCamera->setSize(-halfWidth, halfWidth, -halfHeight, halfHeight);
		}
	}

	void SpriteAnimationPlayer::onPlayPauseClick(bool playing)
	{
		if (mAnimator != nullptr)
		{
			if (playing)
			{
				if (mAnimator->getState() == SpriteAnimationState::Stopped)
				{
					mAnimator->play(mAnimationName, SpriteAnimationType::Left, true, mBaseSpeed);
				}
				else
				{
					mAnimator->pause(false);
				}
			}
			else
			{
				mAnimator->pause(true);
			}
		}
	}

	void SpriteAnimationPlayer::onStopClick()
	{
		if (mAnimator != nullptr)
		{
			mAnimator->stop();
		}
	}

	void SpriteAnimationPlayer::onSpeedChange(float speed)
	{
		if (mAnimator != nullptr)
		{
			mAnimator->setSpeed(kBaseAnimationSpeed * speed);
		}
	}

	void SpriteAnimationPlayer::onLoopClick(bool loop)
	{
		if (mAnimator != nullptr)
		{
			mAnimator->setLooping(loop);
		}
	}
}