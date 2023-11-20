#pragma once

#include "Core/EditorWidget.h"
#include <stdint.h>
#include <memory>

namespace Trinity
{
	class FrameBuffer;
	class Texture;
	class EditorResources;
	class Node;
	class Camera;
	class TextureRenderable;
	class EditorGizmo;

	class Viewport : public EditorWidget
	{
	public:

		Viewport() = default;
		virtual ~Viewport();

		Viewport(const Viewport&) = delete;
		Viewport& operator = (const Viewport&) = delete;

		Viewport(Viewport&&) = default;
		Viewport& operator = (Viewport&&) = default;

		FrameBuffer* getFrameBuffer() const
		{
			return mFrameBuffer.get();
		}

		virtual bool create(uint32_t width, uint32_t height, EditorResources& resources);
		virtual void destroy();

		virtual void setSelectedNode(Node* node);
		virtual void setCamera(Camera& camera);
		virtual void resize(uint32_t width, uint32_t height);
		virtual void draw() override;

	protected:

		virtual void drawGizmoControls(float x, float y);
		virtual void editTransform(float x, float y, float width, float height);

	protected:

		std::unique_ptr<FrameBuffer> mFrameBuffer{ nullptr };
		std::unique_ptr<EditorGizmo> mGizmo{ nullptr };
		Camera* mCamera{ nullptr };
		Node* mSelectedNode{ nullptr };
		TextureRenderable* mRenderable{ nullptr };
		ImVec2 mPosition{ 0.0f, 0.0f };
		ImVec2 mSize{ 0.0f, 0.0f };
	};
}