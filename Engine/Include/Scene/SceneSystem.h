#pragma once

#include "Core/Singleton.h"
#include <memory>
#include <string>

namespace Trinity
{
	class Scene;
	class Camera;
	class BatchRenderer;
	class RenderTarget;
	class RenderPass;
	class ResourceCache;
	class Physics;

	class SceneSystem : public Singleton<SceneSystem>
	{
	public:

		static constexpr const char* kTexturedShader = "/Assets/Engine/Shaders/Textured.wgsl";
		static constexpr const char* kColoredShader = "/Assets/Engine/Shaders/Colored.wgsl";

		SceneSystem() = default;
		virtual ~SceneSystem() = default;

		SceneSystem(const SceneSystem&) = delete;
		SceneSystem& operator = (const SceneSystem&) = delete;

		SceneSystem(SceneSystem&&) = default;
		SceneSystem& operator = (SceneSystem&&) = default;

		BatchRenderer* getRenderer() const
		{
			return mRenderer.get();
		}

		Physics* getPhysics() const
		{
			return mPhysics.get();
		}

		virtual bool create(RenderTarget& renderTarget, ResourceCache& cache);
		virtual void destroy();

		virtual void setScene(Scene& scene);
		virtual void setCamera(Camera& camera);
		virtual void setCamera(const std::string& cameraNodeName);

		virtual void update(float deltaTime);
		virtual void draw(const RenderPass& renderPass);

	protected:

		virtual void drawTextures(const RenderPass& renderPass);
		virtual void drawSprites(const RenderPass& renderPass);

	protected:

		Scene* mScene{ nullptr };
		Camera* mCamera{ nullptr };
		std::unique_ptr<BatchRenderer> mRenderer{ nullptr };
		std::unique_ptr<Physics> mPhysics{ nullptr };
	};
}