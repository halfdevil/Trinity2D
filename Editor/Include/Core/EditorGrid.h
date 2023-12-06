#pragma once

#include "glm/glm.hpp"

namespace Trinity
{
	class RenderTarget;
	class ResourceCache;
	class RenderPipeline;
	class Shader;
	class RenderPass;
	class UniformBuffer;
	class BindGroup;
	class BindGroupLayout;

	class EditorGrid
	{
	public:

		static constexpr const char* kShader = "/Assets/Editor/Shaders/Grid.wgsl";
		static constexpr uint32_t kBindGroupIndex = 0;

		struct GridData
		{
			glm::vec4 thickColor{ 0.7f, 0.7f, 0.7f, 1.0f };
			glm::vec4 thinColor{ 0.3f, 0.3f, 0.3f, 1.0f };
			glm::vec2 resolution{ 0.f };
			glm::vec2 scale{ 0.0f };
		};

		EditorGrid() = default;
		virtual ~EditorGrid();

		EditorGrid(const EditorGrid&) = delete;
		EditorGrid& operator = (const EditorGrid&) = delete;

		EditorGrid(EditorGrid&&) = default;
		EditorGrid& operator = (EditorGrid&&) = default;

		virtual bool create(RenderTarget& renderTarget, ResourceCache& cache);
		virtual void destroy();
		virtual void draw(const RenderPass& renderPass);
		virtual void updateGridData(GridData gridData);

	protected:

		virtual bool createGridBuffer();
		virtual bool createPipeline(RenderTarget& renderTarget);

	protected:

		ResourceCache* mResourceCache{ nullptr };
		RenderPipeline* mPipeline{ nullptr };
		Shader* mShader{ nullptr };
		UniformBuffer* mGridBuffer{ nullptr };		
		BindGroup* mGridBindGroup{ nullptr };
		BindGroupLayout* mGridBindGroupLayout{ nullptr };
	};
}