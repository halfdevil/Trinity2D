#pragma once

#include "Scene/Component.h"
#include <glm/glm.hpp>

namespace Trinity
{
	class Texture;
	class TextureRenderableSerializer;

	class TextureRenderable : public Component
	{
	public:

		friend class TextureRenderableSerializer;

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
		virtual UUIDv4::UUID getUUID() const override;
		virtual Serializer* getSerializer(Scene& scene) override;

		virtual void setTexture(Texture& texture);
		virtual void setOrigin(const glm::vec2& origin);
		virtual void setColor(const glm::vec4& color);
		virtual void setFlip(const glm::bvec2& flip);

	public:

		inline static UUIDv4::UUID UUID = UUIDv4::UUID::fromStrFactory("8224636f-bf93-43f2-af4e-32c11e6ebaab");

	protected:

		Texture* mTexture{ nullptr };
		glm::vec2 mOrigin{ 0.5f };
		glm::vec4 mColor{ 0.0f };
		glm::bvec2 mFlip{ false };
	};

	class TextureRenderableSerializer : public ComponentSerializer
	{
	public:

		TextureRenderableSerializer() = default;
		virtual ~TextureRenderableSerializer() = default;

		TextureRenderableSerializer(const TextureRenderableSerializer&) = delete;
		TextureRenderableSerializer& operator = (const TextureRenderableSerializer&) = delete;

		TextureRenderableSerializer(TextureRenderableSerializer&&) = default;
		TextureRenderableSerializer& operator = (TextureRenderableSerializer&&) = default;

		virtual void setTextureRenderable(TextureRenderable& renderable);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		TextureRenderable* mTextureRenderable{ nullptr };
	};
}