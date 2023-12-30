#pragma once

#include "glm/glm.hpp"

namespace Trinity
{
	class Texture;
	class BatchRenderer;

	class Particle
	{
	public:

		struct TextureFrame
		{
			Texture* texture{ nullptr };
			glm::vec2 position{ 0.0f };
			glm::vec2 size{ 0.0f };
		};

		Particle() = default;
		virtual ~Particle() = default;

		Particle(const Particle&) = delete;
		Particle& operator = (const Particle&) = delete;

		Particle(Particle&&) noexcept = default;
		Particle& operator = (Particle&&) noexcept = default;

		float getLifeRemaining() const
		{
			return mLifeRemaining;
		}

		bool isExpired() const
		{
			return mLifeRemaining < 0.0f;
		}

		virtual void setTexture(Texture& texture, const glm::vec2& position, const glm::vec2& size);
		virtual void clearTexture();

		virtual void setPosition(const glm::vec2& position);
		virtual void setVelocity(const glm::vec2& velocity);
		virtual void setAcceleration(const glm::vec2& acceleration);
		virtual void setStartColor(const glm::vec4& startColor);
		virtual void setFinalColor(const glm::vec4& finalColor);
		virtual void setStartSize(float startSize);
		virtual void setFinalSize(float finalSize);
		virtual void setStartRotation(float startRotation);
		virtual void setFinalRotation(float finalRotation);
		virtual void setDrag(float drag);
		virtual void setLifeSpan(float lifeSpan);

		virtual void update(float deltaTime);
		virtual void draw(BatchRenderer& renderer, const glm::mat4& rootTransform);

	protected:

		TextureFrame mTextureFrame;
		glm::vec2 mPosition{ 0.0f };
		glm::vec2 mVelocity{ 0.0f };
		glm::vec2 mAcceleration{ 0.0f };
		glm::vec4 mStartColor{ 0.0f };
		glm::vec4 mFinalColor{ 0.0f };
		float mStartSize{ 0.0f };
		float mFinalSize{ 0.0f };
		float mStartRotation{ 0.0f };
		float mFinalRotation{ 0.0f };
		float mDrag{ 0.0f };
		float mLifeSpan{ 0.0f };
		float mLifeRemaining{ 0.0f };
	};
}