#include "Scene/Components/SpriteRenderable.h"
#include "Scene/Sprite.h"

namespace Trinity
{
	std::type_index SpriteRenderable::getType() const
	{
		return typeid(SpriteRenderable);
	}

	std::string SpriteRenderable::getTypeName() const
	{
		return getStaticType();
	}

	void SpriteRenderable::setSprite(Sprite& sprite)
	{
		mSprite = &sprite;
	}

	void SpriteRenderable::setOrigin(const glm::vec2& origin)
	{
		mOrigin = origin;
	}

	void SpriteRenderable::setColor(const glm::vec4& color)
	{
		mColor = color;
	}

	void SpriteRenderable::setFlip(const glm::bvec2& flip)
	{
		mFlip = flip;
	}

	void SpriteRenderable::setActiveFrameIndex(uint32_t activeFrameIndex)
	{
		mActiveFrameIndex = activeFrameIndex;
	}

	std::string SpriteRenderable::getStaticType()
	{
		return "SpriteRenderable";
	}
}