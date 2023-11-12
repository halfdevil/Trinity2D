#include "Gui/TextRenderer.h"
#include "Gui/Font.h"
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
#include "Graphics/SwapChain.h"
#include "Core/Logger.h"
#include "Core/ResourceCache.h"
#include "glm/gtx/matrix_decompose.hpp"

namespace Trinity
{
	TextRenderer::~TextRenderer()
	{
		destroy();
	}

	bool TextRenderer::create(const std::string& shaderFile)
	{
		mResourceCache = std::make_unique<ResourceCache>();
		mRenderPass = std::make_unique<RenderPass>();

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

		const SwapChain& swapChain = GraphicsDevice::get().getSwapChain();
		RenderPipelineProperties renderProps = {
			.shader = shader.get(),
			.bindGroupLayouts = {
				mRenderContext.bindGroupLayout,
				mImageContext.bindGroupLayout
			},
			.vertexLayouts = { mRenderContext.vertexLayout },
			.colorTargets = {{
				.format = swapChain.getColorFormat(),
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

		wgpu::TextureFormat depthFormat = swapChain.getDepthFormat();
		if (depthFormat != wgpu::TextureFormat::Undefined)
		{
			renderProps.depthStencil = DepthStencilState{
				.format = depthFormat,
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

	void TextRenderer::destroy()
	{
		mResourceCache->clear();
	}

	void TextRenderer::begin(const glm::mat4& viewProj)
	{
		PerFrameData perFrameData = {
			.viewProj = viewProj
		};

		mRenderContext.perFrameBuffer->write(0, sizeof(PerFrameData), &perFrameData);
	}

	void TextRenderer::end()
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

		mRenderPass->begin();
		mRenderPass->setVertexBuffer(0, *mRenderContext.vertexBuffer);
		mRenderPass->setIndexBuffer(*mRenderContext.indexBuffer);
		mRenderPass->setPipeline(*mRenderContext.pipeline);
		mRenderPass->setBindGroup(kCommonBindGroupIndex, *mRenderContext.bindGroup);

		for (auto& command : mCommands)
		{
			auto& bindGroups = mImageContext.bindGroups;
			if (auto it = bindGroups.find(command.textureId); it != bindGroups.end())
			{
				mRenderPass->setBindGroup(kTextureBindGroupIndex, *it->second);
			}

			mRenderPass->drawIndexed(command.numIndices, 1, command.baseIndex, command.baseVertex);
		}

		mRenderPass->end();
		mRenderPass->submit();

		mStagingContext.numVertices = 0;
		mStagingContext.numIndices = 0;
		mCurrentTexture = nullptr;
		mCommands.clear();
	}

	bool TextRenderer::drawText(const std::string& text, Font* font, float size, const glm::vec4& color,
		const glm::vec2& origin, const glm::mat4& transform)
	{
		auto* texture = font->getTexture();
		auto& charInfos = font->getCharInfos();
		auto offset = font->getLineDistance() - font->getLineGap();

		auto fontWidth = texture->getWidth();
		auto fontHeight = texture->getHeight();

		glm::vec2 textSize{ 0.0f };
		glm::vec3 position{ 0.0f };

		for (auto& c : text)
		{
			auto code = c - 32;
			auto& charInfo = charInfos[code];

			textSize.x += charInfo.xadvance * (size / font->getSize());

			auto height = (charInfo.yoff2 - charInfo.yoff) / fontHeight;
			if (textSize.y < height)
			{
				textSize.y = height;
			}
		}

		glm::vec2 localOrigin {
			textSize.x * origin.x,
			textSize.y * origin.y
		};

		for (auto& c : text)
		{
			auto code = c - 32;
			auto& charInfo = charInfos[code];

			float x1 = position.x + charInfo.xoff - localOrigin.x;
			float y1 = position.y + charInfo.yoff + offset - localOrigin.y;
			float x2 = position.x + charInfo.xoff2 - localOrigin.x;
			float y2 = position.y + charInfo.yoff2 + offset - localOrigin.y;

			glm::vec4 p1 = transform * glm::vec4{ x1, y1, 0.0f, 1.0f };
			glm::vec4 p2 = transform * glm::vec4{ x1, y2, 0.0f, 1.0f };
			glm::vec4 p3 = transform * glm::vec4{ x2, y2, 0.0f, 1.0f };
			glm::vec4 p4 = transform * glm::vec4{ x2, y1, 0.0f, 1.0f };

			glm::vec2 q1 = localOrigin + glm::vec2{ p1.x, p1.y };
			glm::vec2 q2 = localOrigin + glm::vec2{ p2.x, p2.y };
			glm::vec2 q3 = localOrigin + glm::vec2{ p3.x, p3.y };
			glm::vec2 q4 = localOrigin + glm::vec2{ p4.x, p4.y };

			float u1{ (float)charInfo.x0 / (float)fontWidth };
			float v1{ (float)charInfo.y0 / (float)fontHeight };
			float u2{ (float)charInfo.x1 / (float)fontWidth };
			float v2{ (float)charInfo.y1 / (float)fontHeight };

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

			position.x += charInfo.xadvance * (size / font->getSize());
		}

		return true;
	}

	uint32_t TextRenderer::addVertices(const Vertex* vertices, uint32_t numVertices)
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

	uint32_t TextRenderer::addIndices(const uint32_t* indices, uint32_t numIndices)
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

	bool TextRenderer::addCommand(Texture* texture, uint32_t baseVertex,
		uint32_t baseIndex, uint32_t numIndices)
	{
		if (mCurrentTexture != texture)
		{
			if (!createImageBindGroup(*texture))
			{
				LogError("TextRenderer::createImageBindGroup() failed");
				return false;
			}

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

	bool TextRenderer::createCommonBindGroup()
	{
		auto perFrameBuffer = std::make_unique<UniformBuffer>();
		if (!perFrameBuffer->create(sizeof(PerFrameData)))
		{
			LogError("UniformBuffer::create() failed!!");
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

	bool TextRenderer::createImageBindGroup(const Texture& texture)
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

	bool TextRenderer::createBufferData()
	{
		auto vertexLayout = std::make_unique<VertexLayout>();
		vertexLayout->setAttributes({
			{ wgpu::VertexFormat::Float32x2, 0, 0 },
			{ wgpu::VertexFormat::Float32x2, 8, 1 },
			{ wgpu::VertexFormat::Float32x4, 16, 2 }
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