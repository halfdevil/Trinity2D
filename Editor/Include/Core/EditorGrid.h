#pragma once

#include "glm/glm.hpp"
#include <memory>

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
	class LineCanvas;

	class EditorGrid
	{
	public:

		static constexpr const char* kShader = "/Assets/Editor/Shaders/Checkered.wgsl";
		static constexpr uint32_t kBindGroupIndex = 0;

		struct GridData
		{
			glm::vec2 canvasSize{ 0.0f };
			glm::vec2 checkeredSize{ 20.0f };
			glm::vec2 color{ 0.9f, 0.4f };
			glm::vec2 padding;
		};

		struct RenderContext
		{
			Shader* shader{ nullptr };
			RenderPipeline* pipeline{ nullptr };
			UniformBuffer* gridBuffer{ nullptr };
			BindGroup* bindGroup{ nullptr };
			BindGroupLayout* bindGroupLayout{ nullptr };
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

		virtual void setCanvasSize(const glm::vec2& canvasSize);
		virtual void setCheckerSize(const glm::vec2& checkerSize);
		virtual void setColor(float color1, float color2);
		virtual void updateGridData();

	protected:

		virtual bool createBindGroup();
		virtual bool createPipeline(RenderTarget& renderTarget);

	protected:

		ResourceCache* mResourceCache{ nullptr };
		RenderContext mRenderContext;
		GridData mGridData;
	};
}