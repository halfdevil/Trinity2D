#pragma once

#include "Widgets/Viewport.h"

namespace Trinity
{
	class Scene;
	class Camera;
	class Node;
	class SceneSystem;
	class ResourceCache;

	class SceneViewport : public Viewport
	{
	public:

		SceneViewport() = default;
		virtual ~SceneViewport() = default;

		SceneViewport(const SceneViewport&) = delete;
		SceneViewport& operator = (const SceneViewport&) = delete;

		SceneViewport(SceneViewport&&) = default;
		SceneViewport& operator = (SceneViewport&&) = default;

		virtual bool create(EditorResources& resources);
		virtual void destroy() override;

		virtual void setScene(Scene& scene);
		virtual void setSelectedNode(Node* node);
		virtual void drawContent(float deltaTime) override;

	protected:

		virtual void editTransform(float x, float y, float width, float height) override;
		virtual void onViewportResize(uint32_t width, uint32_t height) override;

	protected:

		Scene* mScene{ nullptr };
		Node* mSelectedNode{ nullptr };
		std::unique_ptr<SceneSystem> mSceneSystem{ nullptr };
	};
}