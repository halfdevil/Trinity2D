#pragma once

#include "Scene/Component.h"
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace Trinity
{
	class Sprite;

	class SpriteRenderable : public Component
	{
	public:

		SpriteRenderable() = default;
		virtual ~SpriteRenderable() = default;

		SpriteRenderable(const SpriteRenderable&) = delete;
		SpriteRenderable& operator = (const SpriteRenderable&) = delete;

		SpriteRenderable(SpriteRenderable&&) = default;
		SpriteRenderable& operator = (SpriteRenderable&&) = default;

		Sprite* getSprite() const
		{
			return mSprite;
		}

		const glm::vec2& getOrigin() const
		{
			return mOrigin;
		}

		const glm::vec4& getColor() const
		{
			return mColor;
		}

		const glm::bvec2& getFlip() const
		{
			return mFlip;
		}

		uint32_t getActiveFrameIndex() const
		{
			return mActiveFrameIndex;
		}

		uint32_t getLayer() const
		{
			return mLayer;
		}

		virtual std::type_index getType() const override;
		virtual std::string getTypeName() const override;

		virtual void setSprite(Sprite& sprite);
		virtual void setOrigin(const glm::vec2& origin);
		virtual void setColor(const glm::vec4& color);
		virtual void setFlip(const glm::bvec2& flip);
		virtual void setActiveFrameIndex(uint32_t activeFrameIndex);

	public:

		static std::string getStaticType();

	protected:

		Sprite* mSprite{ nullptr };
		glm::vec2 mOrigin{ 0.5f };
		glm::vec4 mColor{ 0.0f };
		glm::bvec2 mFlip{ false };
		uint32_t mActiveFrameIndex{ 0 };
		uint32_t mLayer{ 0 };
	};
}