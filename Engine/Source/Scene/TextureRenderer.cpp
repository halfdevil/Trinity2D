#include "Scene/TextureRenderer.h"
#include "Scene/Scene.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/TextureRenderable.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/RenderPass.h"
#include "Graphics/RenderTarget.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"

namespace Trinity
{
	bool TextureRenderer::create(RenderTarget& renderTarget)
	{
		mRenderer = std::make_unique<BatchRenderer>();
		if (!mRenderer->create(kShader, renderTarget))
		{
			LogError("BatchRenderer::create() failed with shader: '%s'", kShader);
			return true;
		}

		return true;
	}

	void TextureRenderer::destroy()
	{
		mScene = nullptr;
		mCamera = nullptr;
		mRenderer = nullptr;
	}

	void TextureRenderer::setRotateFromOrigin(bool rotate)
	{
		mRotateFromOrigin = rotate;
	}

	void TextureRenderer::setScene(Scene& scene)
	{
		mScene = &scene;
	}

	void TextureRenderer::setCamera(const std::string& cameraNodeName)
	{
		auto cameraNode = mScene->findNode(cameraNodeName);
		if (cameraNode != nullptr)
		{
			mCamera = &cameraNode->getComponent<Camera>();
		}		
	}

	void TextureRenderer::setCamera(Camera& camera)
	{
		mCamera = &camera;
	}

	void TextureRenderer::draw(const RenderPass& renderPass)
	{
		auto viewProj = mCamera->getProjection() * mCamera->getView();
		auto renderables = mScene->getComponents<TextureRenderable>();

		mRenderer->begin(viewProj);

		for (auto& renderable : renderables)
		{
			auto* texture = renderable->getTexture();
			auto& transform = renderable->getNode()->getTransform();
			auto& flip = renderable->getFlip();
			auto origin = mRotateFromOrigin ? renderable->getOrigin() : glm::vec2{ 0.0f };

			if (texture != nullptr)
			{
				mRenderer->drawTexture(
					texture,
					glm::vec2{ 0.0f, 0.0f },
					glm::vec2{ texture->getWidth(), texture->getHeight() },
					origin,
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