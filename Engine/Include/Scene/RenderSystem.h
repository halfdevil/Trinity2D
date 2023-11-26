#pragma once

#include "Core/Singleton.h"
#include <memory>
#include <string>
#include <typeindex>

namespace Trinity
{
	class Scene;
	class Camera;
	class BatchRenderer;
	class RenderTarget;
	class RenderPass;

	class RenderSystem : public Singleton<RenderSystem>
	{
	public:

		static constexpr const char* kShader = "/Assets/Engine/Shaders/Textured.wgsl";

		RenderSystem() = default;
		virtual ~RenderSystem() = default;

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator = (const RenderSystem&) = delete;

		RenderSystem(RenderSystem&&) = default;
		RenderSystem& operator = (RenderSystem&&) = default;

		BatchRenderer* getRenderer() const
		{
			return mRenderer.get();
		}

		virtual bool create(RenderTarget& renderTarget);
		virtual void destroy();

		virtual void setScene(Scene& scene);
		virtual void setCamera(Camera& camera);
		virtual void setCamera(const std::string& cameraNodeName);

		virtual void draw(const RenderPass& renderPass);

	protected:

		virtual void drawTextures(const RenderPass& renderPass);
		virtual void drawSprites(const RenderPass& renderPass);

	protected:

		Scene* mScene{ nullptr };
		Camera* mCamera{ nullptr };
		std::unique_ptr<BatchRenderer> mRenderer{ nullptr };
	};
}