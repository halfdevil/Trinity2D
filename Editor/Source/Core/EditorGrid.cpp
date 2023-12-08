#include "Core/EditorGrid.h"
#include "Graphics/RenderPass.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/Shader.h"
#include "Graphics/UniformBuffer.h"
#include "Graphics/BindGroup.h"
#include "Graphics/BindGroupLayout.h"
#include "Graphics/LineCanvas.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	EditorGrid::~EditorGrid()
	{
		destroy();
	}

	bool EditorGrid::create(RenderTarget& renderTarget, ResourceCache& cache)
	{
		mResourceCache = &cache;

		if (!createBindGroup())
		{
			LogError("EditorGrid::createBindGroup() failed!!");
			return false;
		}

		if (!createPipeline(renderTarget))
		{
			LogError("EditorGrid::createPipeline() failed");
			return false;
		}

		return true;
	}

	void EditorGrid::destroy()
	{
		mResourceCache->removeResource(mRenderContext.pipeline);
		mResourceCache->removeResource(mRenderContext.shader);
		mResourceCache->removeResource(mRenderContext.gridBuffer);
		mResourceCache->removeResource(mRenderContext.bindGroup);
		mResourceCache->removeResource(mRenderContext.bindGroupLayout);
		mRenderContext = {};
	}

	void EditorGrid::draw(const RenderPass& renderPass)
	{
		renderPass.setBindGroup(kBindGroupIndex, *mRenderContext.bindGroup);
		renderPass.setPipeline(*mRenderContext.pipeline);
		renderPass.draw(6);
	}

	void EditorGrid::setCanvasSize(const glm::vec2& canvasSize)
	{
		mGridData.canvasSize = canvasSize;
	}

	void EditorGrid::setCheckerSize(const glm::vec2& checkerSize)
	{
		mGridData.checkeredSize = checkerSize;
	}

	void EditorGrid::setColor(float color1, float color2)
	{
		mGridData.color = glm::vec2{ color1, color2 };
	}

	void EditorGrid::updateGridData()
	{
		mRenderContext.gridBuffer->write(0, sizeof(GridData), &mGridData);
	}

	bool EditorGrid::createBindGroup()
	{
		auto gridBuffer = std::make_unique<UniformBuffer>();
		if (!gridBuffer->create(sizeof(GridData)))
		{
			LogError("UniformBuffer::create() failed!!");
			return false;
		}

		const std::vector<BindGroupLayoutItem> layoutItems =
		{
			{
				.binding = 0,
				.shaderStages = wgpu::ShaderStage::Fragment,
				.bindingLayout = BufferBindingLayout {
					.type = wgpu::BufferBindingType::Uniform,
					.minBindingSize = sizeof(GridData)
				}
			}
		};

		const std::vector<BindGroupItem> bindGroupItems =
		{
			{
				.binding = 0,
				.size = sizeof(GridData),
				.resource = BufferBindingResource(*gridBuffer)
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

		mRenderContext.gridBuffer = gridBuffer.get();
		mRenderContext.bindGroupLayout = bindGroupLayout.get();
		mRenderContext.bindGroup = bindGroup.get();

		mResourceCache->addResource(std::move(gridBuffer));
		mResourceCache->addResource(std::move(bindGroupLayout));
		mResourceCache->addResource(std::move(bindGroup));

		return true;
	}

	bool EditorGrid::createPipeline(RenderTarget& renderTarget)
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
}