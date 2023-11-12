#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Trinity
{
	class Font;
	class RenderPass;
	class Texture;
	class Sampler;
	class ResourceCache;
	class Shader;
	class RenderPipeline;
	class UniformBuffer;
	class BindGroup;
	class BindGroupLayout;
	class VertexLayout;
	class VertexBuffer;
	class IndexBuffer;

	class TextRenderer
	{
	public:

		static constexpr const char* kDefaultShader = "/Assets/Engine/Shaders/TextRenderer.wgsl";
		static constexpr uint32_t kCommonBindGroupIndex = 0;
		static constexpr uint32_t kTextureBindGroupIndex = 1;

		struct Vertex
		{
			glm::vec2 position{ 0.0f };
			glm::vec2 uv{ 0.0f };
			glm::vec4 color{ 0.0f };
		};

		struct PerFrameData
		{
			glm::mat4 viewProj;
		};

		struct DrawCommand
		{
			size_t textureId{ 0 };
			uint32_t numIndices{ 0 };
			uint32_t baseIndex{ 0 };
			uint32_t baseVertex{ 0 };
		};

		struct RenderContext
		{
			Shader* shader{ nullptr };
			RenderPipeline* pipeline{ nullptr };
			VertexLayout* vertexLayout{ nullptr };
			VertexBuffer* vertexBuffer{ nullptr };
			IndexBuffer* indexBuffer{ nullptr };
			UniformBuffer* perFrameBuffer{ nullptr };
			BindGroup* bindGroup{ nullptr };
			BindGroupLayout* bindGroupLayout{ nullptr };
		};

		struct ImageContext
		{
			Sampler* sampler{ nullptr };
			BindGroupLayout* bindGroupLayout{ nullptr };
			std::unordered_map<size_t, BindGroup*> bindGroups;
		};

		struct StagingContext
		{
			uint32_t numVertices{ 0 };
			uint32_t numIndices{ 0 };
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
		};

	public:

		TextRenderer() = default;
		~TextRenderer();

		TextRenderer(const TextRenderer&) = delete;
		TextRenderer& operator = (const TextRenderer&) = delete;

		TextRenderer(TextRenderer&&) = default;
		TextRenderer& operator = (TextRenderer&&) = default;

		virtual bool create(const std::string& shaderFile = kDefaultShader);
		virtual void destroy();

		virtual void begin(const glm::mat4& viewProj);
		virtual void end();

		virtual bool drawText(const std::string& text, Font* font, float size, const glm::vec4& color,
			const glm::vec2& origin, const glm::mat4& transform);

	protected:

		virtual uint32_t addVertices(const Vertex* vertices, uint32_t numVertices);
		virtual uint32_t addIndices(const uint32_t* indices, uint32_t numIndices);

		virtual bool addCommand(Texture* texture, uint32_t baseVertex,
			uint32_t baseIndex, uint32_t numIndices);

		virtual bool createCommonBindGroup();
		virtual bool createImageBindGroup(const Texture& texture);
		virtual bool createBufferData();

	protected:

		RenderContext mRenderContext;
		ImageContext mImageContext;
		StagingContext mStagingContext;
		std::unique_ptr<ResourceCache> mResourceCache{ nullptr };
		std::unique_ptr<RenderPass> mRenderPass{ nullptr };
		Texture* mCurrentTexture{ nullptr };
		std::vector<DrawCommand> mCommands;
	};
}