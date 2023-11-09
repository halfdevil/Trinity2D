#include "Scene/Sprite.h"

namespace Trinity
{
	SpriteFrame* Sprite::getFrame(uint32_t idx)
	{
		if (idx < (uint32_t)mFrames.size())
		{
			return &mFrames[idx];
		}

		return nullptr;
	}

	SpriteAnimation* Sprite::getAnimation(uint32_t idx)
	{
		if (idx < (uint32_t)mAnimations.size())
		{
			return &mAnimations[idx];
		}

		return nullptr;
	}

	SpriteAnimation* Sprite::getAnimation(const std::string& name)
	{
		for (auto& animation : mAnimations)
		{
			if (animation.name == name)
			{
				return &animation;
			}
		}

		return nullptr;
	}

	void Sprite::setSize(const glm::vec2& size)
	{
		mSize = size;
	}

	void Sprite::setTexture(Texture& texture)
	{
		mTexture = &texture;
	}

	void Sprite::setFrames(std::vector<SpriteFrame>&& frames)
	{
		mFrames = std::move(frames);
	}

	void Sprite::setAnimations(std::vector<SpriteAnimation>&& animations)
	{
		mAnimations = std::move(animations);
	}
}