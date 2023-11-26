#include "Scene/RenderSystem.h"
#include "Scene/Scene.h"
#include "Scene/Sprite.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/SpriteRenderable.h"
#include "Scene/Components/TextureRenderable.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/RenderPass.h"
#include "Graphics/RenderTarget.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"

namespace Trinity
{
	bool RenderSystem::create(RenderTarget& renderTarget, ResourceCache& cache)
	{
		mRenderer = std::make_unique<BatchRenderer>();
		if (!mRenderer->create(kShader, renderTarget, cache))
		{
			LogError("BatchRenderer::create() failed with shader: '%s'", kShader);
			return true;
		}

		return true;
	}

	void RenderSystem::destroy()
	{
		mScene = nullptr;
		mCamera = nullptr;
		mRenderer = nullptr;
	}

	void RenderSystem::setScene(Scene& scene)
	{
		mScene = &scene;
	}

	void RenderSystem::setCamera(Camera& camera)
	{
		mCamera = &camera;
	}

	void RenderSystem::setCamera(const std::string& cameraNodeName)
	{
		auto cameraNode = mScene->findNode(cameraNodeName);
		if (cameraNode != nullptr)
		{
			mCamera = &cameraNode->getComponent<Camera>();
		}
	}

	void RenderSystem::draw(const RenderPass& renderPass)
	{
		if (mCamera != nullptr)
		{
			drawTextures(renderPass);
			drawSprites(renderPass);
		}
	}

	void RenderSystem::drawTextures(const RenderPass& renderPass)
	{
		auto viewProj = mCamera->getProjection() * mCamera->getView();
		auto renderables = mScene->getComponents<TextureRenderable>();

		mRenderer->begin(viewProj);

		for (auto& renderable : renderables)
		{
			if (!renderable->isActive())
			{
				continue;
			}

			auto* texture = renderable->getTexture();
			auto& transform = renderable->getNode()->getTransform();
			auto& flip = renderable->getFlip();

			if (texture != nullptr)
			{
				mRenderer->drawTexture(
					texture,
					glm::vec2{ 0.0f, 0.0f },
					glm::vec2{ texture->getWidth(), texture->getHeight() },
					renderable->getOrigin(),
					transform.getWorldMatrix(),
					renderable->getColor(),
					flip.x,
					flip.y
				);
			}
		}

		mRenderer->end(renderPass);
	}

	void RenderSystem::drawSprites(const RenderPass& renderPass)
	{
		auto viewProj = mCamera->getProjection() * mCamera->getView();
		auto renderables = mScene->getComponents<SpriteRenderable>();

		std::sort(renderables.begin(), renderables.end(), [](const auto& a, const auto& b) {
			return a->getLayer() > b->getLayer();
		});

		mRenderer->begin(viewProj);

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

		mRenderer->end(renderPass);
	}
}