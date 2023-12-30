#pragma once

#include "Core/EditorWidget.h"
#include "Core/Observer.h"
#include <string>
#include <memory>

namespace Trinity
{
	class ParticleEffect;
	class ParticleEffect;

	class ParticleHierarchy : public EditorWidget
	{
	public:

		ParticleHierarchy() = default;
		virtual ~ParticleHierarchy() = default;

		ParticleHierarchy(const ParticleHierarchy&) = delete;
		ParticleHierarchy& operator = (const ParticleHierarchy&) = delete;

		ParticleHierarchy(ParticleHierarchy&&) = default;
		ParticleHierarchy& operator = (ParticleHierarchy&&) = default;

		uint32_t getSelectedEmitter() const
		{
			return mSelectedEmitter;
		}

		virtual void setParticle(ParticleEffect& sprite);
		virtual void setSelectedEmitter(uint32_t selectedEmitter);
		virtual void draw() override;

	public:

		Observer<uint32_t> onSelectEmitter;

	protected:

		ParticleEffect* mParticle{ nullptr };
		uint32_t mSelectedEmitter{ 0 };
	};
}