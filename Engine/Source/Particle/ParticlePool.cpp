#include "Particle/ParticlePool.h"
#include "Particle/Particle.h"

namespace Trinity
{
	Particle& ParticlePool::getParticleSprite()
	{
		if (mNumActive == mNumParticles)
		{
			resize(mNumParticles * 2);
		}

		return mParticleSprites.at(mNumActive++);
	}

	void ParticlePool::init(uint32_t poolSize)
	{
		resize(poolSize);
	}

	void ParticlePool::update(float deltaTime)
	{
		for (uint32_t idx = 0; idx < mNumActive; idx++)
		{
			auto& sprite = mParticleSprites[idx];
			sprite.update(deltaTime);
		}

		for (uint32_t idx = 0; idx < mNumActive; idx++)
		{
			auto& sprite = mParticleSprites[idx];
			if (sprite.isExpired())
			{
				if (idx != mNumActive - 1)
				{
					auto& lastActive = mParticleSprites[mNumActive - 1];
					std::swap(sprite, lastActive);
				}

				mNumActive--;
				idx--;
			}
		}
	}

	void ParticlePool::draw(BatchRenderer& renderer, const glm::mat4& rootTransform)
	{
		for (uint32_t idx = 0; idx < mNumActive; idx++)
		{
			auto& sprite = mParticleSprites[idx];
			sprite.draw(renderer, rootTransform);
		}
	}

	void ParticlePool::resize(uint32_t newSize)
	{
		mParticleSprites.resize(newSize);
		mNumParticles = newSize;
	}
}