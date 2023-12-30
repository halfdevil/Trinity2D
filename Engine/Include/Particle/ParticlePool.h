#pragma once

#include "Particle/Particle.h"
#include <memory>
#include <vector>
#include "glm/glm.hpp"

namespace Trinity
{
	class BatchRenderer;

	class ParticlePool
	{
	public:

		ParticlePool() = default;
		virtual ~ParticlePool() = default;

		ParticlePool(const ParticlePool&) = delete;
		ParticlePool& operator = (const ParticlePool&) = delete;

		ParticlePool(ParticlePool&&) = default;
		ParticlePool& operator = (ParticlePool&&) = default;

		virtual Particle& getParticleSprite();

		virtual void init(uint32_t poolSize);
		virtual void update(float deltaTime);
		virtual void draw(BatchRenderer& renderer, const glm::mat4& rootTransform);

	protected:

		virtual void resize(uint32_t newSize);

	protected:

		uint32_t mNumParticles{ 0 };
		uint32_t mNumActive{ 0 };
		std::vector<Particle> mParticleSprites;
	};
}