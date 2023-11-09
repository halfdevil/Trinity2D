#pragma once

#include "Core/Resource.h"
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace Trinity
{
	class Texture;

	struct SpriteFrame
	{
		std::string name;
		glm::vec2 position{ 0.0f };
		glm::vec2 size{ 0.0f };
	};

	struct SpriteAnimation
	{
		std::string name;
		std::vector<uint16_t> frames;
	};

	class Sprite : public Resource
	{
	public:

		Sprite() = default;
		virtual ~Sprite() = default;

		Sprite(const Sprite&) = delete;
		Sprite& operator = (const Sprite&) = delete;

		Sprite(Sprite&&) = default;
		Sprite& operator = (Sprite&&) = default;

		const glm::vec2& getSize() const
		{
			return mSize;
		}

		Texture* getTexture() const
		{
			return mTexture;
		}

		const std::vector<SpriteFrame>& getFrames() const
		{
			return mFrames;
		}

		const std::vector<SpriteAnimation>& getAnimations() const
		{
			return mAnimations;
		}

		virtual SpriteFrame* getFrame(uint32_t idx);
		virtual SpriteAnimation* getAnimation(uint32_t idx);
		virtual SpriteAnimation* getAnimation(const std::string& name);

		virtual void setSize(const glm::vec2& size);
		virtual void setTexture(Texture& texture);
		virtual void setFrames(std::vector<SpriteFrame>&& frames);
		virtual void setAnimations(std::vector<SpriteAnimation>&& animations);

	protected:

		glm::vec2 mSize{ 0.0f };
		Texture* mTexture{ nullptr };
		std::vector<SpriteFrame> mFrames;
		std::vector<SpriteAnimation> mAnimations;
	};
}