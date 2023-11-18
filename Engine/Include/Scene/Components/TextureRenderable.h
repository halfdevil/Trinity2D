#pragma once

#include "Scene/Component.h"
#include <glm/glm.hpp>

namespace Trinity
{
	class Texture;

	class TextureRenderable : public Component
	{
	public:

		TextureRenderable() = default;
		virtual ~TextureRenderable() = default;

		TextureRenderable(const TextureRenderable&) = delete;
		TextureRenderable& operator = (const TextureRenderable&) = delete;

		TextureRenderable(TextureRenderable&&) = default;
		TextureRenderable& operator = (TextureRenderable&&) = default;

		Texture* getTexture() const
		{
			return mTexture;
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

		virtual std::type_index getType() const override;
		virtual std::string getTypeName() const override;

		virtual void setTexture(Texture& texture);
		virtual void setOrigin(const glm::vec2& origin);
		virtual void setColor(const glm::vec4& color);
		virtual void setFlip(const glm::bvec2& flip);

	public:

		static std::string getStaticType();

	protected:

		Texture* mTexture{ nullptr };
		glm::vec2 mOrigin{ 0.5f };
		glm::vec4 mColor{ 0.0f };
		glm::bvec2 mFlip{ false };
	};
}