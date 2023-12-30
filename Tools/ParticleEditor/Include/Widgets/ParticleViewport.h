#pragma once

#include "Widgets/Viewport.h"

namespace Trinity
{
	class ParticleEffect;

	class ParticleViewport : public Viewport
	{
	public:

		static constexpr const char* kTexturedShader = "/Assets/Engine/Shaders/Particle.wgsl";
		static constexpr const char* kColoredShader = "/Assets/Engine/Shaders/Colored.wgsl";

		ParticleViewport() = default;
		virtual ~ParticleViewport() = default;

		ParticleViewport(const ParticleViewport&) = delete;
		ParticleViewport& operator = (const ParticleViewport&) = delete;

		ParticleViewport(ParticleViewport&&) = default;
		ParticleViewport& operator = (ParticleViewport&&) = default;

		virtual bool create(EditorResources& resources);
		virtual void destroy() override;

		virtual void setParticle(ParticleEffect& sprite);
		virtual void update(float deltaTime) override;
		virtual void drawContent(float deltaTime) override;

	protected:

		virtual void onViewportResize(uint32_t width, uint32_t height) override;

	protected:

		ParticleEffect* mParticle{ nullptr };
	};
}