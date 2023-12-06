#pragma once

#include "Core/Singleton.h"
#include "glm/glm.hpp"
#include <string>
#include <vector>

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
	class VertexBuffer;
	class IndexBuffer;
	class VertexLayout;
	struct BoundingRect;

	class DebugHelper : public Singleton<DebugHelper>
	{
	public:

		static constexpr const char* kShader = "/Assets/Engine/Shaders/Debug.wgsl";
		static constexpr uint32_t kBindGroupIndex = 0;

		struct Vertex
		{
			glm::vec2 position{ 0.0f };
			glm::vec4 color{ 0.0f, 0.0f, 0.0f, 1.0f };
		};

		struct PerFrameData
		{
			glm::mat4 viewProj{ 1.0f };
		};

		struct RenderContext
		{
			RenderPipeline* pipeline{ nullptr };
			Shader* shader{ nullptr };
			VertexLayout* vertexLayout{ nullptr };
			VertexBuffer* vertexBuffer{ nullptr };
			IndexBuffer* indexBuffer{ nullptr };
			UniformBuffer* perFrameBuffer{ nullptr };
			BindGroup* bindGroup{ nullptr };
			BindGroupLayout* bindGroupLayout{ nullptr };
		};

		struct StagingContext
		{
			uint32_t numVertices{ 0 };
			uint32_t numIndices{ 0 };
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
		};

		DebugHelper() = default;
		virtual ~DebugHelper();

		DebugHelper(const DebugHelper&) = delete;
		DebugHelper& operator = (const DebugHelper&) = delete;

		DebugHelper(DebugHelper&&) = default;
		DebugHelper& operator = (DebugHelper&&) = default;

		virtual bool create(RenderTarget& renderTarget, ResourceCache& cache);
		virtual void destroy();

		virtual void line(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color, float lineWidth = 1.0f);
		virtual void box(const BoundingRect& rect, const glm::vec4& color, float lineWidth = 1.0f);
		virtual void draw(const glm::mat4& viewProj, const RenderPass& renderPass);

	protected:

		virtual bool createBufferData();
		virtual bool createBindGroup();
		virtual bool createPipeline(RenderTarget& renderTarget);

		virtual void addVertices(const Vertex* vertices, uint32_t numVertices);
		virtual void addIndices(const uint32_t* indices, uint32_t numIndices);

	protected:
			
		ResourceCache* mResourceCache{ nullptr };
		RenderContext mRenderContext;
		StagingContext mStagingContext;
	};
}