#pragma once

#include "Core/EditorWidget.h"
#include "Core/Observer.h"
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
	class Camera;
	class EditorGizmo;

	class Viewport : public EditorWidget
	{
	public:

		struct Resolution
		{
			std::string name;
			uint32_t width{ 0 };
			uint32_t height{ 0 };
		};

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

		Resolution* getSelectedResolution() const
		{
			return mSelectedResolution;
		}

		virtual bool create(EditorResources& resources);
		virtual void destroy();
				
		virtual void setCamera(Camera& camera);
		virtual void resize(uint32_t width, uint32_t height);

		virtual void drawContent(float deltaTime);
		virtual void draw() override;

	protected:
		
		virtual void drawControls(float x, float y);
		virtual void editTransform(float x, float y, float width, float height);

	public:

		Observer<uint32_t, uint32_t> onResize;

	protected:

		std::unique_ptr<FrameBuffer> mFrameBuffer{ nullptr };
		std::unique_ptr<EditorGizmo> mGizmo{ nullptr };
		std::unique_ptr<RenderPass> mRenderPass{ nullptr };
		std::vector<Resolution> mResolutions;
		Resolution* mSelectedResolution{ nullptr };
		Camera* mCamera{ nullptr };
		ImVec2 mPosition{ 0.0f, 0.0f };
		ImVec2 mSize{ 0.0f, 0.0f };
	};
}