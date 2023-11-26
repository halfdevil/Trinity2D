#pragma once

#include "Core/EditorWidget.h"
#include "glm/glm.hpp"
#include <stdint.h>
#include <memory>
#include <vector>

namespace Trinity
{
	class FrameBuffer;
	class Texture;
	class RenderSystem;
	class RenderPass;
	class EditorResources;
	class Scene;
	class Node;
	class Camera;
	class EditorGizmo;

	class SceneViewport : public EditorWidget
	{
	public:

		struct Resolution
		{
			std::string name;
			uint32_t width{ 0 };
			uint32_t height{ 0 };
		};

		SceneViewport() = default;
		virtual ~SceneViewport();

		SceneViewport(const SceneViewport&) = delete;
		SceneViewport& operator = (const SceneViewport&) = delete;

		SceneViewport(SceneViewport&&) = default;
		SceneViewport& operator = (SceneViewport&&) = default;

		FrameBuffer* getFrameBuffer() const
		{
			return mFrameBuffer.get();
		}

		virtual bool create(EditorResources& resources);
		virtual void destroy();

		virtual void setScene(Scene& scene);
		virtual void setCamera(Camera& camera);
		virtual void setSelectedNode(Node* node);

		virtual void resize(uint32_t width, uint32_t height);
		virtual void drawScene(float deltaTime);
		virtual void draw() override;

	protected:

		virtual void drawControls(float x, float y);
		virtual void editTransform(float x, float y, float width, float height);

	protected:

		std::unique_ptr<FrameBuffer> mFrameBuffer{ nullptr };
		std::unique_ptr<EditorGizmo> mGizmo{ nullptr };
		std::unique_ptr<RenderSystem> mRenderSystem{ nullptr };
		std::unique_ptr<RenderPass> mRenderPass{ nullptr };
		std::vector<Resolution> mResolutions;
		Resolution* mSelectedResolution{ nullptr };
		Scene* mScene{ nullptr };
		Node* mSelectedNode{ nullptr };
		Camera* mCamera{ nullptr };
		ImVec2 mPosition{ 0.0f, 0.0f };
		ImVec2 mSize{ 0.0f, 0.0f };
	};
}