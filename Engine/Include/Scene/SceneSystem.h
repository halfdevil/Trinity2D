#pragma once

#include "Core/Singleton.h"
#include "Scene/QuadTree.h"
#include "Math/BoundingRect.h"
#include <memory>
#include <string>
#include "glm/glm.hpp"

namespace Trinity
{
	class Scene;
	class Camera;
	class BatchRenderer;
	class RenderTarget;
	class RenderPass;
	class ResourceCache;
	class Physics;
	class Collider;
	struct ColliderData;

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
		virtual void setupQuadTree(const BoundingRect& sceneBounds, const BoundingRect& minBounds);

		virtual void update(float deltaTime);
		virtual void draw(const RenderPass& renderPass);
		virtual void draw(const RenderPass& renderPass, const glm::mat4& viewProj);

	protected:

		virtual void updateQuadTree(Collider& collider);
		virtual void queryColliders(Collider& collider, std::vector<Collider*>& colliders);
		virtual void collision(Collider& collider);

		virtual void drawTextures(const RenderPass& renderPass, const glm::mat4& viewProj);
		virtual void drawSprites(const RenderPass& renderPass, const glm::mat4& viewProj);

	protected:

		Scene* mScene{ nullptr };
		Camera* mCamera{ nullptr };
		std::unique_ptr<BatchRenderer> mRenderer{ nullptr };
		std::unique_ptr<Physics> mPhysics{ nullptr };
		std::unique_ptr<QuadTree> mQuadTree{ nullptr };
	};
}