#include "Particle/Particle.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/compatibility.hpp"

namespace Trinity
{
	void Particle::setTexture(Texture& texture, const glm::vec2& position, const glm::vec2& size)
	{
		mTextureFrame = {
			.texture = &texture,
			.position = position,
			.size = size
		};
	}

	void Particle::clearTexture()
	{
		mTextureFrame = {};
	}

	void Particle::setPosition(const glm::vec2& position)
	{
		mPosition = position;
	}

	void Particle::setVelocity(const glm::vec2& velocity)
	{
		mVelocity = velocity;
	}

	void Particle::setAcceleration(const glm::vec2& acceleration)
	{
		mAcceleration = acceleration;
	}

	void Particle::setStartColor(const glm::vec4& startColor)
	{
		mStartColor = startColor;
	}

	void Particle::setFinalColor(const glm::vec4& finalColor)
	{
		mFinalColor = finalColor;
	}

	void Particle::setStartSize(float startSize)
	{
		mStartSize = startSize;
	}

	void Particle::setFinalSize(float finalSize)
	{
		mFinalSize = finalSize;
	}

	void Particle::setStartRotation(float startRotation)
	{
		mStartRotation = startRotation;
	}

	void Particle::setFinalRotation(float finalRotation)
	{
		mFinalRotation = finalRotation;
	}

	void Particle::setDrag(float drag)
	{
		mDrag = drag;
	}

	void Particle::setLifeSpan(float lifeSpan)
	{
		mLifeSpan = lifeSpan;
		mLifeRemaining = lifeSpan;
	}

	void Particle::update(float deltaTime)
	{
		const auto deltaTimeSecs = deltaTime / 1000.0f;
		mLifeRemaining -= deltaTimeSecs;

		mVelocity += mAcceleration * deltaTimeSecs;
		mVelocity *= mDrag;
		mPosition += mVelocity * deltaTimeSecs;
	}

	void Particle::draw(BatchRenderer& renderer, const glm::mat4& rootTransform)
	{
		const auto life = mLifeRemaining / mLifeSpan;
		const auto size = glm::lerp(mFinalSize, mStartSize, life);
		const auto color = glm::lerp(mFinalColor, mStartColor, life);
		const auto rotation = glm::lerp(mFinalRotation, mStartRotation, life);

		const auto transform = rootTransform *
			glm::translate(glm::mat4(1.0f), glm::vec3(mPosition, 0.0f)) * 
			glm::yawPitchRoll(0.0f, 0.0f, rotation);

		if (mTextureFrame.texture != nullptr)
		{
			renderer.drawTexture(
				mTextureFrame.texture, 
				mTextureFrame.position, 
				mTextureFrame.size, 
				glm::vec2{ 0.0f }, 
				{ size, size },
				glm::vec2{ 0.5f }, 
				transform, 
				color
			);
		}
		else
		{
			renderer.drawRect(
				glm::vec2{ 0.0f }, 
				{ size, size },
				glm::vec2{0.5f}, 
				transform, 
				color
			);
		}
	}
}