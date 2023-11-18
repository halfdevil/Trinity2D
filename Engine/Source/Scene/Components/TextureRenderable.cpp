#include "Scene/Components/TextureRenderable.h"
#include "Graphics/Texture.h"

namespace Trinity
{
	std::type_index TextureRenderable::getType() const
	{
		return typeid(TextureRenderable);
	}

	std::string TextureRenderable::getTypeName() const
	{
		return getStaticType();
	}

	void TextureRenderable::setTexture(Texture& texture)
	{
		mTexture = &texture;
	}

	void TextureRenderable::setOrigin(const glm::vec2& origin)
	{
		mOrigin = origin;
	}

	void TextureRenderable::setColor(const glm::vec4& color)
	{
		mColor = color;
	}

	void TextureRenderable::setFlip(const glm::bvec2& flip)
	{
		mFlip = flip;
	}

	std::string TextureRenderable::getStaticType()
	{
		return "TextureRenderable";
	}
}