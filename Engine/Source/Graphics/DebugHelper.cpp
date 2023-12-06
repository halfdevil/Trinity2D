#include "Graphics/DebugHelper.h"
#include "Graphics/RenderPass.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/Shader.h"
#include "Graphics/UniformBuffer.h"
#include "Graphics/BindGroup.h"
#include "Graphics/BindGroupLayout.h"
#include "Math/BoundingRect.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include <array>

namespace Trinity
{
	DebugHelper::~DebugHelper()
	{
		destroy();
	}

	bool DebugHelper::create(RenderTarget& renderTarget, ResourceCache& cache)
	{
		mResourceCache = &cache;

		if (!createBufferData())
		{
			LogError("EditorGrid::createGridBuffer() failed");
			return false;
		}

		if (!createPipeline(renderTarget))
		{
			LogError("EditorGrid::createPipeline() failed");
			return false;
		}

		return true;
	}

	void DebugHelper::destroy()
	{
		mResourceCache->removeResource(mRenderContext.pipeline);
		mResourceCache->removeResource(mRenderContext.shader);
		mResourceCache->removeResource(mRenderContext.vertexLayout);
		mResourceCache->removeResource(mRenderContext.vertexBuffer);
		mResourceCache->removeResource(mRenderContext.indexBuffer);
		mResourceCache->removeResource(mRenderContext.perFrameBuffer);
		mResourceCache->removeResource(mRenderContext.bindGroup);
		mResourceCache->removeResource(mRenderContext.bindGroupLayout);
		
		mRenderContext = {};
		mStagingContext = {};
	}

	void DebugHelper::line(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color, float lineWidth)
	{
		float x1{ p1.x };
		float y1{ p1.y };
		float x2{ x1 + lineWidth };
		float y2{ y1 + lineWidth };

		auto p1 = glm::vec4{ x1, y1, 0.0f, 1.0f };
		auto p2 = glm::vec4{ x1, y2, 0.0f, 1.0f };
		auto p3 = glm::vec4{ x2, y2, 0.0f, 1.0f };
		auto p4 = glm::vec4{ x2, y1, 0.0f, 1.0f };

		Vertex vertices[4] = {
			{ .position = glm::vec2{ x1, y1 }, .color = color },
			{ .position = glm::vec2{ x1, y2 }, .color = color },
			{ .position = glm::vec2{ x2, y2 }, .color = color },
			{ .position = glm::vec2{ x2, y1 }, .color = color }
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

		addVertices(vertices, 4);
		addIndices(indices, 6);
	}

	void DebugHelper::box(const BoundingRect& rect, const glm::vec4& c, float lineWidth)
	{
		std::array<glm::vec2, 4> points{
			glm::vec2(rect.min.x, rect.min.y),
			glm::vec2(rect.min.x, rect.max.y),
			glm::vec2(rect.max.x, rect.max.y),
			glm::vec2(rect.max.x, rect.min.y)
		};

		line(points[0], points[1], c, lineWidth);
		line(points[1], points[2], c, lineWidth);
		line(points[2], points[3], c, lineWidth);
		line(points[3], points[4], c, lineWidth);
	}

	void DebugHelper::draw(const glm::mat4& viewProj, const RenderPass& renderPass)
	{
		PerFrameData perFrameData = {
			.viewProj = viewProj
		};

		mRenderContext.perFrameBuffer->write(0, sizeof(PerFrameData), &perFrameData);

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
		renderPass.setBindGroup(kBindGroupIndex, *mRenderContext.bindGroup);
		renderPass.setPipeline(*mRenderContext.pipeline);
		renderPass.drawIndexed(mStagingContext.numIndices);

		mStagingContext.numVertices = 0;
		mStagingContext.numIndices = 0;
	}

	bool DebugHelper::createBufferData()
	{
		auto vertexLayout = std::make_unique<VertexLayout>();
		vertexLayout->setAttributes({
			{ wgpu::VertexFormat::Float32x2, 0, 0 },
			{ wgpu::VertexFormat::Float32x4, 8, 2 }
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

	bool DebugHelper::createBindGroup()
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

		mRenderContext.perFrameBuffer = perFrameBuffer.get();
		mRenderContext.bindGroupLayout = bindGroupLayout.get();
		mRenderContext.bindGroup = bindGroup.get();

		mResourceCache->addResource(std::move(perFrameBuffer));
		mResourceCache->addResource(std::move(bindGroupLayout));
		mResourceCache->addResource(std::move(bindGroup));

		return true;
	}

	bool DebugHelper::createPipeline(RenderTarget& renderTarget)
	{
		ShaderPreProcessor processor;

		auto shader = std::make_unique<Shader>();
		if (!shader->create(kShader, processor))
		{
			LogError("Shader::create() failed for: %s", kShader);
			return false;
		}

		RenderPipelineProperties renderProps = {
			.shader = shader.get(),
			.bindGroupLayouts = {
				mRenderContext.bindGroupLayout
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

		mRenderContext.pipeline = pipeline.get();
		mResourceCache->addResource(std::move(shader));
		mResourceCache->addResource(std::move(pipeline));

		return true;
	}

	void DebugHelper::addVertices(const Vertex* vertices, uint32_t numVertices)
	{
		auto& allVertices = mStagingContext.vertices;
		if (mStagingContext.numVertices + numVertices > (uint32_t)allVertices.size())
		{
			allVertices.resize(allVertices.size() + 5000);
		}

		std::memcpy(&allVertices[mStagingContext.numVertices], vertices, sizeof(Vertex) * numVertices);
		mStagingContext.numVertices += numVertices;
	}

	void DebugHelper::addIndices(const uint32_t* indices, uint32_t numIndices)
	{
		auto& allIndices = mStagingContext.indices;
		if (mStagingContext.numIndices + numIndices > (uint32_t)allIndices.size())
		{
			allIndices.resize(allIndices.size() + 10000);
		}

		std::memcpy(&allIndices[mStagingContext.numIndices], indices, sizeof(uint32_t) * numIndices);
		mStagingContext.numIndices += numIndices;
	}
}