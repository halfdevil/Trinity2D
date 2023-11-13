#include "Graphics/BatchRenderer.h"
#include "Graphics/RenderPass.h"
#include "Graphics/Texture.h"
#include "Graphics/Sampler.h"
#include "Graphics/Shader.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/UniformBuffer.h"
#include "Graphics/BindGroup.h"
#include "Graphics/BindGroupLayout.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/UniformBuffer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/SwapChain.h"
#include "Core/Logger.h"
#include "Core/ResourceCache.h"
#include "glm/gtx/matrix_decompose.hpp"

namespace Trinity
{
	BatchRenderer::~BatchRenderer()
	{
		destroy();
	}

	bool BatchRenderer::create(const std::string& shaderFile, RenderTarget& renderTarget)
	{
		mResourceCache = std::make_unique<ResourceCache>();

		if (!createBufferData())
		{
			LogError("createBufferData() failed!!");
			return false;
		}

		if (!createCommonBindGroup())
		{
			LogError("createCommonBindGroup() failed!!");
			return false;
		}

		ShaderPreProcessor processor;
		auto shader = std::make_unique<Shader>();
		if (!shader->create(shaderFile, processor))
		{
			LogError("Shader::create() failed for: %s!!", shaderFile.c_str());
			return false;
		}

		RenderPipelineProperties renderProps = {
			.shader = shader.get(),
			.bindGroupLayouts = {
				mRenderContext.bindGroupLayout,
				mImageContext.bindGroupLayout
			},
			.vertexLayouts = { mRenderContext.vertexLayout },
			.colorTargets = {{
				.format = renderTarget.getColorFormat(),
				.blendState = wgpu::BlendState {
					.color = {
						.operation = wgpu::BlendOperation::Add,
						.srcFactor = wgpu::BlendFactor::SrcAlpha,
						.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha
					},
					.alpha = {
						.operation = wgpu::BlendOperation::Add,
						.srcFactor = wgpu::BlendFactor::One,
						.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha
					}
				}
			}},
			.primitive = {
				.topology = wgpu::PrimitiveTopology::TriangleList,
				.frontFace = wgpu::FrontFace::CW,
				.cullMode = wgpu::CullMode::None
			}
		};

		if (renderTarget.hasDepthStencilAttachment())
		{
			renderProps.depthStencil = {
				.format = renderTarget.getDepthFormat(),
				.depthWriteEnabled = false,
				.depthCompare = wgpu::CompareFunction::Less
			};
		}

		auto pipeline = std::make_unique<RenderPipeline>();
		if (!pipeline->create(renderProps))
		{
			LogError("RenderPipeline::create() failed!!");
			return false;
		}

		mRenderContext.shader = shader.get();
		mRenderContext.pipeline = pipeline.get();
		mResourceCache->addResource(std::move(shader));
		mResourceCache->addResource(std::move(pipeline));

		return true;
	}

	void BatchRenderer::destroy()
	{
		mResourceCache->clear();
	}

	bool BatchRenderer::drawTexture(
		Texture* texture, 
		const glm::vec2& srcPosition, 
		const glm::vec2& srcSize, 
		const glm::vec2& origin, 
		const glm::mat4& transform, 
		const glm::vec4& color,
		bool flipX, 
		bool flipY
	)
	{
		glm::vec3 localOrigin = { 
			srcSize.x * origin.x, 
			srcSize.y * origin.y,
			0.0f
		};

		float x1{ -localOrigin.x };
		float y1{ -localOrigin.y };
		float x2{ x1 + srcSize.x };
		float y2{ y1 + srcSize.y };

		glm::vec4 p1 = transform * glm::vec4{ x1, y1, 0.0f, 1.0f };
		glm::vec4 p2 = transform * glm::vec4{ x1, y2, 0.0f, 1.0f };
		glm::vec4 p3 = transform * glm::vec4{ x2, y2, 0.0f, 1.0f };
		glm::vec4 p4 = transform * glm::vec4{ x2, y1, 0.0f, 1.0f };

		glm::vec3 q1 = localOrigin + glm::vec3{ p1.x, p1.y, p1.z };
		glm::vec3 q2 = localOrigin + glm::vec3{ p2.x, p2.y, p2.z };
		glm::vec3 q3 = localOrigin + glm::vec3{ p3.x, p3.y, p3.z };
		glm::vec3 q4 = localOrigin + glm::vec3{ p4.x, p4.y, p4.z };

		float u1{ srcPosition.x * mInvTextureSize.x };
		float v1{ srcPosition.y * mInvTextureSize.y };
		float u2{ (srcPosition.x + srcSize.x) * mInvTextureSize.x };
		float v2{ (srcPosition.y + srcSize.y) * mInvTextureSize.y };

		if (flipX)
		{
			auto t = u1;
			u1 = u2;
			u2 = t;
		}

		if (flipY)
		{
			auto t = v1;
			v1 = v2;
			v2 = t;
		}

		Vertex vertices[4] = {
			{.position = q1, .uv = { u1, v1 }, .color = color },
			{.position = q2, .uv = { u1, v2 }, .color = color },
			{.position = q3, .uv = { u2, v2 }, .color = color },
			{.position = q4, .uv = { u2, v1 }, .color = color }
		};

		auto numVertices = mStagingContext.numVertices;
		uint32_t indices[6] = {
			numVertices,
			numVertices + 1,
			numVertices + 2,
			numVertices + 2,
			numVertices + 3,
			numVertices
		};

		if (!addCommand(texture, addVertices(vertices, 4), addIndices(indices, 6), 6))
		{
			LogError("BatchRenderer::addCommand() failed");
			return false;
		}

		return true;
	}

	bool BatchRenderer::drawTexture(
		Texture* texture, 
		const glm::vec2& srcPosition, 
		const glm::vec2& srcSize, 
		const glm::vec2& dstPosition, 
		const glm::vec2& dstSize, 
		const glm::vec4& color,
		float depth,
		bool flipX, 
		bool flipY
	)
	{
		mInvTextureSize = {
			1.0f / (float)texture->getWidth(),
			1.0f / (float)texture->getHeight()
		};

		float x1{ dstPosition.x };
		float y1{ dstPosition.y };
		float x2{ dstPosition.x + dstSize.x };
		float y2{ dstPosition.y + dstSize.y };

		float u1{ srcPosition.x * mInvTextureSize.x };
		float v1{ srcPosition.y * mInvTextureSize.y };
		float u2{ (srcPosition.x + srcSize.x) * mInvTextureSize.x };
		float v2{ (srcPosition.y + srcSize.y) * mInvTextureSize.y };

		if (flipX)
		{
			auto t = u1;
			u1 = u2;
			u2 = t;
		}

		if (flipY)
		{
			auto t = v1;
			v1 = v2;
			v2 = t;
		}

		Vertex vertices[4] = {
			{ .position = { x1, y1, depth }, .uv = { u1, v1 }, .color = color },
			{ .position = { x1, y2, depth }, .uv = { u1, v2 }, .color = color },
			{ .position = { x2, y2, depth }, .uv = { u2, v2 }, .color = color },
			{ .position = { x2, y1, depth }, .uv = { u2, v1 }, .color = color }
		};

		auto numVertices = mStagingContext.numVertices;
		uint32_t indices[6] = {
			numVertices,
			numVertices + 1,
			numVertices + 2,
			numVertices + 2,
			numVertices + 3,
			numVertices
		};

		if (!addCommand(texture, addVertices(vertices, 4), addIndices(indices, 6), 6))
		{
			LogError("BatchRenderer::addCommand() failed");
			return false;
		}

		return true;
	}

	void BatchRenderer::begin(const glm::mat4& viewProj)
	{
		PerFrameData perFrameData = {
			.viewProj = viewProj
		};

		mRenderContext.perFrameBuffer->write(0, sizeof(PerFrameData), &perFrameData);
	}

	void BatchRenderer::end(const RenderPass& renderPass)
	{
		auto* vertexBuffer = mRenderContext.vertexBuffer;
		if (vertexBuffer->getNumVertices() < mStagingContext.numVertices)
		{
			vertexBuffer->destroy();
			if (!vertexBuffer->create(*mRenderContext.vertexLayout, mStagingContext.numVertices))
			{
				LogError("VertexBuffer::create() failed");
				return;
			}
		}

		auto* indexBuffer = mRenderContext.indexBuffer;
		if (indexBuffer->getNumIndices() < mStagingContext.numIndices)
		{
			indexBuffer->destroy();
			if (!indexBuffer->create(wgpu::IndexFormat::Uint32, mStagingContext.numIndices))
			{
				LogError("IndexBuffer::create() failed");
				return;
			}
		}

		vertexBuffer->write(0, sizeof(Vertex) * mStagingContext.numVertices,
			mStagingContext.vertices.data());

		indexBuffer->write(0, sizeof(uint32_t) * mStagingContext.numIndices,
			mStagingContext.indices.data());

		renderPass.setVertexBuffer(0, *mRenderContext.vertexBuffer);
		renderPass.setIndexBuffer(*mRenderContext.indexBuffer);
		renderPass.setPipeline(*mRenderContext.pipeline);
		renderPass.setBindGroup(kCommonBindGroupIndex, *mRenderContext.bindGroup);

		for (auto& command : mCommands)
		{
			auto& bindGroups = mImageContext.bindGroups;
			if (auto it = bindGroups.find(command.textureId); it != bindGroups.end())
			{
				renderPass.setBindGroup(kTextureBindGroupIndex, *it->second);
			}

			renderPass.drawIndexed(command.numIndices, 1, command.baseIndex, command.baseVertex);
		}

		mStagingContext.numVertices = 0;
		mStagingContext.numIndices = 0;
		mCurrentTexture = nullptr;
		mCommands.clear();
	}

	uint32_t BatchRenderer::addVertices(const Vertex* vertices, uint32_t numVertices)
	{
		auto& allVertices = mStagingContext.vertices;
		auto baseVertex = mStagingContext.numVertices;

		if (mStagingContext.numVertices + numVertices > (uint32_t)allVertices.size())
		{
			allVertices.resize(allVertices.size() + 5000);
		}

		std::memcpy(&allVertices[mStagingContext.numVertices], vertices, sizeof(Vertex) * numVertices);
		mStagingContext.numVertices += numVertices;

		return baseVertex;
	}

	uint32_t BatchRenderer::addIndices(const uint32_t* indices, uint32_t numIndices)
	{
		auto& allIndices = mStagingContext.indices;
		auto baseIndex = mStagingContext.numIndices;

		if (mStagingContext.numIndices + numIndices > (uint32_t)allIndices.size())
		{
			allIndices.resize(allIndices.size() + 10000);
		}

		std::memcpy(&allIndices[mStagingContext.numIndices], indices, sizeof(uint32_t) * numIndices);
		mStagingContext.numIndices += numIndices;

		return baseIndex;
	}

	bool BatchRenderer::addCommand(Texture* texture, uint32_t baseVertex, 
		uint32_t baseIndex, uint32_t numIndices)
	{
		if (mCurrentTexture != texture)
		{
			if (!createImageBindGroup(*texture))
			{
				LogError("BatchRenderer::createImageBindGroup() failed");
				return false;
			}

			mInvTextureSize = {
				1.0f / (float)texture->getWidth(),
				1.0f / (float)texture->getHeight()
			};

			DrawCommand drawCommand = {
				.textureId = std::hash<const Texture*>{}(texture),
				.numIndices = numIndices,
				.baseIndex = baseIndex,
				.baseVertex = baseVertex
			};

			mCommands.push_back(drawCommand);
			mCurrentTexture = texture;
		}
		else
		{
			auto& command = mCommands.back();
			command.numIndices += numIndices;
		}	

		return true;
	}

	bool BatchRenderer::createCommonBindGroup()
	{
		auto perFrameBuffer = std::make_unique<UniformBuffer>();
		if (!perFrameBuffer->create(sizeof(PerFrameData)))
		{
			LogError("UniformBuffer::create() failed!!");
			return false;
		}

		const std::vector<BindGroupLayoutItem> layoutItems =
		{
			{
				.binding = 0,
				.shaderStages = wgpu::ShaderStage::Vertex,
				.bindingLayout = BufferBindingLayout {
					.type = wgpu::BufferBindingType::Uniform,
					.minBindingSize = sizeof(PerFrameData)
				}
			}
		};

		const std::vector<BindGroupItem> bindGroupItems =
		{
			{
				.binding = 0,
				.size = sizeof(PerFrameData),
				.resource = BufferBindingResource(*perFrameBuffer)
			}
		};

		auto bindGroupLayout = std::make_unique<BindGroupLayout>();
		if (!bindGroupLayout->create(layoutItems))
		{
			LogError("BindGroupLayout::create() failed!!");
			return false;
		}

		auto bindGroup = std::make_unique<BindGroup>();
		if (!bindGroup->create(*bindGroupLayout, bindGroupItems))
		{
			LogError("BindGroup::create() failed!!");
			return false;
		}

		const std::vector<BindGroupLayoutItem> imageLayoutItems =
		{
			{
				.binding = 0,
				.shaderStages = wgpu::ShaderStage::Fragment,
				.bindingLayout = SamplerBindingLayout {
					.type = wgpu::SamplerBindingType::Filtering
				}
			},
			{
				.binding = 1,
				.shaderStages = wgpu::ShaderStage::Fragment,
				.bindingLayout = TextureBindingLayout {
					.sampleType = wgpu::TextureSampleType::Float,
					.viewDimension = wgpu::TextureViewDimension::e2D
				}
			}
		};

		auto imageBindGroupLayout = std::make_unique<BindGroupLayout>();
		if (!imageBindGroupLayout->create(imageLayoutItems))
		{
			LogError("BindGroupLayout::create() failed!!");
			return false;
		}

		mRenderContext.perFrameBuffer = perFrameBuffer.get();
		mRenderContext.bindGroupLayout = bindGroupLayout.get();
		mRenderContext.bindGroup = bindGroup.get();
		mImageContext.bindGroupLayout = imageBindGroupLayout.get();

		mResourceCache->addResource(std::move(perFrameBuffer));
		mResourceCache->addResource(std::move(bindGroupLayout));
		mResourceCache->addResource(std::move(bindGroup));
		mResourceCache->addResource(std::move(imageBindGroupLayout));

		return true;
	}

	bool BatchRenderer::createImageBindGroup(const Texture& texture)
	{
		if (mImageContext.sampler == nullptr)
		{
			auto sampler = std::make_unique<Sampler>();
			if (!sampler->create({
				.addressModeU = wgpu::AddressMode::Repeat,
				.addressModeV = wgpu::AddressMode::Repeat,
				.addressModeW = wgpu::AddressMode::Repeat,
				.magFilter = wgpu::FilterMode::Linear,
				.minFilter = wgpu::FilterMode::Linear,
				.mipmapFilter = wgpu::MipmapFilterMode::Linear
			}))
			{
				LogError("Sampler::create() failed!!");
				return false;
			}

			mImageContext.sampler = sampler.get();
			mResourceCache->addResource(std::move(sampler));
		}

		const std::vector<BindGroupItem> bindGroupItems =
		{
			{
				.binding = 0,
				.resource = SamplerBindingResource(*mImageContext.sampler)
			},
			{
				.binding = 1,
				.resource = TextureBindingResource(texture)
			}
		};

		auto bindGroup = std::make_unique<BindGroup>();
		if (!bindGroup->create(*mImageContext.bindGroupLayout, bindGroupItems))
		{
			LogError("BindGroup::create() failed!!");
			return false;
		}

		size_t textureId = std::hash<const Texture*>{}(&texture);
		mImageContext.bindGroups.insert(std::make_pair(textureId, bindGroup.get()));
		mResourceCache->addResource(std::move(bindGroup));

		return true;
	}

	bool BatchRenderer::createBufferData()
	{
		auto vertexLayout = std::make_unique<VertexLayout>();
		vertexLayout->setAttributes({
			{ wgpu::VertexFormat::Float32x3, 0, 0 },
			{ wgpu::VertexFormat::Float32x2, 12, 1 },
			{ wgpu::VertexFormat::Float32x4, 20, 2 }
		});

		uint32_t numVertices = 128 * 1024;
		uint32_t numIndices = 256 * 1024;

		auto vertexBuffer = std::make_unique<VertexBuffer>();
		if (!vertexBuffer->create(*vertexLayout, numVertices))
		{
			LogError("VertexBuffer::create() failed!!");
			return false;
		}

		auto indexBuffer = std::make_unique<IndexBuffer>();
		if (!indexBuffer->create(wgpu::IndexFormat::Uint32, numIndices))
		{
			LogError("IndexBuffer::create() failed!!");
			return false;
		}

		mStagingContext.vertices.resize(numVertices);
		mStagingContext.indices.resize(numIndices);

		mRenderContext.vertexLayout = vertexLayout.get();
		mRenderContext.vertexBuffer = vertexBuffer.get();
		mRenderContext.indexBuffer = indexBuffer.get();

		mResourceCache->addResource(std::move(vertexLayout));
		mResourceCache->addResource(std::move(vertexBuffer));
		mResourceCache->addResource(std::move(indexBuffer));

		return true;
	}
}