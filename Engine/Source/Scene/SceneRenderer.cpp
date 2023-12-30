#include "Scene/SceneRenderer.h"
#include "Scene/Node.h"
#include "Scene/Sprite.h"
#include "Scene/Components/Light.h"
#include "Scene/Components/TextureRenderable.h"
#include "Scene/Components/SpriteRenderable.h"
#include "Graphics/RenderPass.h"
#include "Graphics/Texture.h"
#include "Graphics/Sampler.h"
#include "Graphics/Shader.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/UniformBuffer.h"
#include "Graphics/StorageBuffer.h"
#include "Graphics/BindGroup.h"
#include "Graphics/BindGroupLayout.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/UniformBuffer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/SwapChain.h"
#include "Graphics/RenderTarget.h"
#include "Core/Logger.h"
#include "Core/ResourceCache.h"
#include "glm/gtx/matrix_decompose.hpp"

namespace Trinity
{
	bool SceneRenderer::create(RenderTarget& renderTarget, ResourceCache& cache)
	{
		if (!createLightBindGroup())
		{
			LogError("SceneRenderer::createLightBindGroup() failed");
			return false;
		}

		return BatchRenderer::create(renderTarget, cache, kTexturedShader, kColoredShader, 
			{ mLightContext.bindGroupLayout }, sizeof(SceneData));
	}

	void SceneRenderer::drawRenderable(TextureRenderable& renderable)
	{
		auto* texture = renderable.getTexture();
		auto& transform = renderable.getNode()->getTransform();
		auto& flip = renderable.getFlip();

		if (texture != nullptr)
		{
			drawTexture(
				texture,
				glm::vec2{ 0.0f, 0.0f },
				glm::vec2{ texture->getWidth(), texture->getHeight() },
				renderable.getOrigin(),
				transform.getWorldMatrix(),
				renderable.getColor(),
				flip.x,
				flip.y
			);
		}
	}

	void SceneRenderer::drawRenderable(SpriteRenderable& renderable)
	{
		auto* sprite = renderable.getSprite();
		auto& transform = renderable.getNode()->getTransform();
		auto& flip = renderable.getFlip();

		if (sprite != nullptr)
		{
			auto* frame = sprite->getFrame(renderable.getActiveFrameIndex());			
			drawTexture(
				sprite->getTexture(),
				frame->position,
				frame->size,
				renderable.getOrigin(),
				transform.getWorldMatrix(),
				renderable.getColor(),
				flip.x,
				flip.y
			);
		}
	}

	void SceneRenderer::setCustomBindGroups(const RenderPass& renderPass)
	{
		renderPass.setBindGroup(kLightBindGroupIndex, *mLightContext.bindGroup);
	}

	void SceneRenderer::updatePerFrameBuffer(const glm::mat4& viewProj)
	{
		SceneData sceneData = {
			.viewProj = viewProj,
			.numLights = (uint32_t)mLightContext.lights.size()
		};

		mRenderContext.perFrameBuffer->write(0, sizeof(SceneData), &sceneData);
	}

	bool SceneRenderer::createLightBindGroup()
	{
		auto lightsBuffer = std::make_unique<StorageBuffer>();
		if (!lightsBuffer->create(kMaxLights * sizeof(LightData)))
		{
			LogError("StorageBuffer::create() failed for lights!!");
			return false;
		}

		std::vector<BindGroupLayoutItem> lightLayoutItems = {
			{
				.binding = 1,
				.shaderStages = wgpu::ShaderStage::Fragment,
				.bindingLayout = BufferBindingLayout {
					.type = wgpu::BufferBindingType::ReadOnlyStorage,
					.minBindingSize = kMaxLights * (uint32_t)sizeof(LightData)
				}
			}
		};

		std::vector<BindGroupItem> lightItems = {
			{
				.binding = 1,
				.size = kMaxLights * sizeof(LightData),
				.resource = BufferBindingResource(*lightsBuffer)
			}
		};

		auto bindGroupLayout = std::make_unique<BindGroupLayout>();
		if (!bindGroupLayout->create(lightLayoutItems))
		{
			LogError("BindGroupLayout::create() failed!!");
			return false;
		}

		auto bindGroup = std::make_unique<BindGroup>();
		if (!bindGroup->create(*bindGroupLayout, lightItems))
		{
			LogError("BindGroup::create() failed!!");
			return false;
		}

		mLightContext.buffer = lightsBuffer.get();
		mLightContext.bindGroup = bindGroup.get();
		mLightContext.bindGroupLayout = bindGroupLayout.get();

		mResourceCache->addResource(std::move(lightsBuffer));
		mResourceCache->addResource(std::move(bindGroup));
		mResourceCache->addResource(std::move(bindGroupLayout));

		return true;
	}
}