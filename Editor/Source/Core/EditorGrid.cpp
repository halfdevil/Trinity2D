#include "Core/EditorGrid.h"
#include "Graphics/RenderPass.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/Shader.h"
#include "Graphics/UniformBuffer.h"
#include "Graphics/BindGroup.h"
#include "Graphics/BindGroupLayout.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"

namespace Trinity
{
	EditorGrid::~EditorGrid()
	{
		destroy();
	}

	bool EditorGrid::create(RenderTarget& renderTarget, ResourceCache& cache)
	{
		mResourceCache = &cache;

		if (!createGridBuffer())
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

	void EditorGrid::destroy()
	{
		if (mPipeline != nullptr)
		{
			mResourceCache->removeResource(mPipeline);
		}

		if (mShader != nullptr)
		{
			mResourceCache->removeResource(mShader);
		}

		if (mGridBuffer != nullptr)
		{
			mResourceCache->removeResource(mGridBuffer);
		}

		if (mGridBindGroup != nullptr)
		{
			mResourceCache->removeResource(mGridBindGroup);
		}

		if (mGridBindGroupLayout != nullptr)
		{
			mResourceCache->removeResource(mGridBindGroupLayout);
		}

		mPipeline = nullptr;
		mShader = nullptr;
		mGridBuffer = nullptr;
		mGridBindGroup = nullptr;
		mGridBindGroupLayout = nullptr;
	}

	void EditorGrid::draw(const RenderPass& renderPass)
	{
		renderPass.setBindGroup(kBindGroupIndex, *mGridBindGroup);
		renderPass.setPipeline(*mPipeline);
		renderPass.draw(4);
	}

	void EditorGrid::updateGridData(GridData gridData)
	{
		if (mGridBuffer != nullptr)
		{
			mGridBuffer->write(0, sizeof(GridData), &gridData);
		}
	}

	bool EditorGrid::createGridBuffer()
	{
		GridData gridData{};

		auto gridDataBuffer = std::make_unique<UniformBuffer>();
		if (!gridDataBuffer->create(sizeof(GridData), &gridData))
		{
			LogError("UniformBuffer::create() failed!!");
			return false;
		}

		const std::vector<BindGroupLayoutItem> layoutItems =
		{
			{
				.binding = 0,
				.shaderStages = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment,
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
				.resource = BufferBindingResource(*gridDataBuffer)
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

		mGridBuffer = gridDataBuffer.get();
		mGridBindGroup = bindGroup.get();
		mGridBindGroupLayout = bindGroupLayout.get();

		mResourceCache->addResource(std::move(gridDataBuffer));
		mResourceCache->addResource(std::move(bindGroup));
		mResourceCache->addResource(std::move(bindGroupLayout));

		return true;
	}

	bool EditorGrid::createPipeline(RenderTarget& renderTarget)
	{
		ShaderPreProcessor processor;

		auto shader = std::make_unique<Shader>();
		if (!shader->create(kShader, processor))
		{
			LogError("Shader::create() failed for: %s!!", kShader);
			return false;
		}

		RenderPipelineProperties renderProps = {
			.shader = shader.get(),
			.bindGroupLayouts = {
				mGridBindGroupLayout
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
				.topology = wgpu::PrimitiveTopology::TriangleStrip,
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

		mShader = shader.get();
		mPipeline = pipeline.get();

		mResourceCache->addResource(std::move(shader));
		mResourceCache->addResource(std::move(pipeline));

		return true;
	}
}