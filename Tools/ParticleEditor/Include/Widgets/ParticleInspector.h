#pragma once

#include "Widgets/Inspector.h"

namespace Trinity
{
	class ParticleEffect;

	class ParticleInspector : public Inspector
	{
	public:

		ParticleInspector() = default;
		virtual ~ParticleInspector() = default;

		ParticleInspector(const ParticleInspector&) = delete;
		ParticleInspector& operator = (const ParticleInspector&) = delete;

		ParticleInspector(ParticleInspector&&) = default;
		ParticleInspector& operator = (ParticleInspector&&) = default;

		virtual void setParticle(ParticleEffect& sprite);
		virtual void setSelectedEmitter(uint32_t selectedEmitter);

	protected:

		virtual void drawInspector() override;

	protected:

		ParticleEffect* mParticle{ nullptr };
		uint32_t mSelectedEmitter{ 0 };
	};
}