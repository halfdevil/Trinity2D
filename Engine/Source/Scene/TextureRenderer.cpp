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
	bool TextureRenderer::create(Scene& scene, RenderTarget& renderTarget)
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

	void TextureRenderer::destroy()
	{
		mScene = nullptr;
		mCamera = nullptr;
		mRenderer = nullptr;
	}

	void TextureRenderer::setCamera(const std::string& nodeName)
	{
		auto cameraNode = mScene->findNode(nodeName);
		if (!cameraNode)
		{
			LogWarning("Camera node '%s' not found. Looking for 'default_camera' node", nodeName.c_str());
			cameraNode = mScene->findNode("default_camera");
		}

		if (cameraNode != nullptr)
		{
			mCamera = &cameraNode->getComponent<Camera>();
		}
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
}