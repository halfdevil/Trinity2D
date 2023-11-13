#include "Scene/SpriteRenderer.h"
#include "Scene/Scene.h"
#include "Scene/Sprite.h"
#include "Scene/Components/SpriteRenderable.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/RenderPass.h"
#include "Graphics/RenderTarget.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"

namespace Trinity
{
	bool SpriteRenderer::create(Scene& scene, RenderTarget& renderTarget)
	{
		mScene = &scene;
		mRenderer = std::make_unique<BatchRenderer>();

		if (!mRenderer->create(kShader, renderTarget))
		{
			LogError("BatchRenderer::create() failed with shader: '%s'", kShader);
			return true;
		}

		return true;
	}

	void SpriteRenderer::destroy()
	{
		mScene = nullptr;
		mRenderer = nullptr;
	}

	void SpriteRenderer::draw(float deltaTime)
	{
		auto renderables = mScene->getComponents<SpriteRenderable>();
		std::sort(renderables.begin(), renderables.end(), [](const auto& a, const auto& b) {
			return a->getLayer() > b->getLayer();
		});

		for (auto& renderable : renderables)
		{
			auto* sprite = renderable->getSprite();
			auto& transform = renderable->getNode()->getTransform();
			auto& flip = renderable->getFlip();

			auto activeIndex = renderable->getActiveFrameIndex();
			auto* frame = sprite->getFrame(activeIndex);

			if (frame != nullptr)
			{
				mRenderer->drawTexture(
					sprite->getTexture(), 
					frame->position, 
					frame->size, 
					renderable->getOrigin(), 
					transform.getWorldMatrix(), 
					renderable->getColor(), 
					flip.x, 
					flip.y
				);
			}
		}
	}
}