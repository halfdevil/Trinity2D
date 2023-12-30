#include "Widgets/ParticleViewport.h"
#include "Particle/ParticleEffect.h"
#include "Particle/ParticleEmitter.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderPass.h"
#include "Core/EditorResources.h"
#include "Core/EditorCamera.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"

namespace Trinity
{	
	bool ParticleViewport::create(EditorResources& resources)
	{
		if (!Viewport::create(resources, 0, { 0.0f, 0.0f, 0.0f, 1.0f }, kTexturedShader, kColoredShader))
		{
			LogError("Viewport::create() failed");
			return false;
		}

		mShowTopToolbar = true;
		mShowBottomToolbar = false;
		mShowGizmoControls = false;
		mShowResolutionControls = true;

		return true;
	}

	void ParticleViewport::destroy()
	{
		Viewport::destroy();
		mParticle = nullptr;
	}

	void ParticleViewport::setParticle(ParticleEffect& particle)
	{
		mParticle = &particle;
	}

	void ParticleViewport::update(float deltaTime)
	{
		Viewport::update(deltaTime);

		if (mParticle != nullptr)
		{
			mParticle->update(deltaTime);
		}
	}

	void ParticleViewport::drawContent(float deltaTime)
	{
		mRenderPass->begin(*mFrameBuffer);

		if (mParticle != nullptr)
		{
			mRenderer->begin(mCamera->getViewProj());
			mParticle->draw(*mRenderer);
			mRenderer->end(*mRenderPass);
		}

		mRenderPass->end();
	}

	void ParticleViewport::onViewportResize(uint32_t width, uint32_t height)
	{
		Viewport::onViewportResize(width, height);

		if (mCamera != nullptr)
		{
			float halfWidth = 0.5f * width;
			float halfHeight = 0.5f * height;

			mCamera->setSize(-halfWidth, halfWidth, -halfHeight, halfHeight);
		}
	}
}