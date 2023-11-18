#pragma once

#include <memory>
#include <string>

namespace Trinity
{
	class BatchRenderer;
	class Scene;
	class Camera;
	class RenderTarget;
	class RenderPass;

	class TextureRenderer
	{
	public:

		static constexpr const char* kShader = "/Assets/Engine/Shaders/TextureRenderer.wgsl";

		TextureRenderer() = default;
		virtual ~TextureRenderer() = default;

		TextureRenderer(const TextureRenderer&) = delete;
		TextureRenderer& operator = (const TextureRenderer&) = delete;

		TextureRenderer(TextureRenderer&&) = default;
		TextureRenderer& operator = (TextureRenderer&&) = default;

		virtual bool create(RenderTarget& renderTarget);
		virtual void destroy();

		virtual void setScene(Scene& scene, const std::string& cameraNodeName);
		virtual void draw(const RenderPass& renderPass);

	protected:

		Scene* mScene{ nullptr };
		Camera* mCamera{ nullptr };
		std::unique_ptr<BatchRenderer> mRenderer{ nullptr };
	};
}