#pragma once

#include "Graphics/BatchRenderer.h"

namespace Trinity
{
	class Scene;
	class Light;
	class TextureRenderable;
	class SpriteRenderable;
	class StorageBuffer;
	class BindGroupLayout;
	class BindGroup;

	class SceneRenderer : public BatchRenderer
	{
	public:

		static constexpr const char* kTexturedShader = "/Assets/Engine/Shaders/Textured.wgsl";
		static constexpr const char* kColoredShader = "/Assets/Engine/Shaders/Colored.wgsl";
		static constexpr uint32_t kLightBindGroupIndex = 2;
		static constexpr uint32_t kMaxLights = 16;

		struct SceneData
		{
			glm::mat4 viewProj{ 1.0f };
			uint32_t numLights{ 0 };
			glm::vec3 padding{ 0.0f };
		};

		struct LightData
		{
			glm::vec3 position{ 0.0f };
			float intensity{ 0.0f };
			glm::vec4 color{ 0.0f };
			float near{ 0.0f };
			float far{ 0.0f };
			glm::vec2 padding{ 0.0f };
		};

		struct LightContext
		{
			std::vector<Light*> lights;
			StorageBuffer* buffer{ nullptr };
			BindGroupLayout* bindGroupLayout{ nullptr };
			BindGroup* bindGroup{ nullptr };
		};

		SceneRenderer() = default;
		virtual ~SceneRenderer() = default;

		SceneRenderer(const SceneRenderer&) = delete;
		SceneRenderer& operator = (const SceneRenderer&) = delete;

		SceneRenderer(SceneRenderer&&) = default;
		SceneRenderer& operator = (SceneRenderer&&) = default;

		virtual bool create(RenderTarget& renderTarget, ResourceCache& cache);

		virtual void addLight(Light* light);

		virtual void drawRenderable(TextureRenderable& renderable);
		virtual void drawRenderable(SpriteRenderable& renderable);

	protected:

		virtual void setCustomBindGroups(const RenderPass& renderPass) override;
		virtual void updatePerFrameBuffer(const glm::mat4& viewProj) override;
		virtual bool createLightBindGroup();

	protected:

		LightContext mLightContext;
	};
}