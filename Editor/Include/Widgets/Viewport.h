#pragma once

#include "Core/EditorWidget.h"
#include "Core/Observer.h"
#include "Input/Types.h"
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
	class LineCanvas;
	class BatchRenderer;
	class EditorResources;
	class EditorGizmo;
	class EditorCamera;
	class EditorGrid;

	class Viewport : public EditorWidget
	{
	public:

		static constexpr const char* kTexturedShader = "/Assets/Engine/Shaders/Textured.wgsl";
		static constexpr const char* kColoredShader = "/Assets/Engine/Shaders/Colored.wgsl";
		static constexpr float kTopToolbarHeight = 24.0f;
		static constexpr float kBottomToolbarHeight = 24.0f;
		static constexpr float kZoomSpeed = 0.1f;
		static constexpr float kZoomMax = 8.0f;
		static constexpr float kZoomMin = 0.25f;
		static constexpr float kPanSpeed = 0.01f;

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

		LineCanvas* getLineCanvas() const
		{
			return mLineCanvas.get();
		}

		EditorCamera* getCamera() const
		{
			return mCamera.get();
		}

		Resolution* getSelectedResolution() const
		{
			return mSelectedResolution;
		}

		const glm::vec2& getPosition() const
		{
			return mPosition;
		}

		const glm::vec2& getSize() const
		{
			return mSize;
		}

		virtual bool create(
			EditorResources& resources, 
			uint32_t resolutionIndex = 0, 
			const glm::vec4& clearColor = glm::vec4{ 0.5f, 0.5f, 0.5f, 1.0f },
			const std::string& texturedShader = kTexturedShader,
			const std::string& coloredShader = kColoredShader
		);

		virtual void destroy();				
		virtual void resize(uint32_t width, uint32_t height);
		virtual void drawContent(float deltaTime);
		virtual void update(float deltaTime);
		virtual void draw() override;

	protected:

		virtual void drawTopToolbar(float x, float y, float width, float height);
		virtual void drawBottomToolbar(float x, float y, float width, float height);
		virtual void editTransform(float x, float y, float width, float height);
		virtual void drawTopToolbarWidgets();
		virtual void drawBottomToolbarWidgets();

		virtual void onViewportResize(uint32_t width, uint32_t height);
		virtual void onMousePositionUpdated(float x, float y);
		virtual void onMouseScrollUpdated(float x, float y);
		virtual void onMouseButtonStateUpdated(int32_t button, bool pressed);
		virtual glm::vec2 convertToViewport(const glm::vec2& v) const;

	public:

		Observer<uint32_t, uint32_t> onResize;

	protected:

		std::unique_ptr<FrameBuffer> mFrameBuffer{ nullptr };
		std::unique_ptr<RenderPass> mRenderPass{ nullptr };
		std::unique_ptr<BatchRenderer> mRenderer{ nullptr };
		std::unique_ptr<LineCanvas> mLineCanvas{ nullptr };
		std::unique_ptr<EditorGizmo> mGizmo{ nullptr };
		std::unique_ptr<EditorCamera> mCamera{ nullptr };
		std::unique_ptr<EditorGrid> mGrid{ nullptr };
		std::vector<Resolution> mResolutions;
		Resolution* mSelectedResolution{ nullptr };
		bool mHorizontalScrollBar{ false };
		bool mKeepAspectRatio{ true };
		bool mShowTopToolbar{ true };
		bool mShowBottomToolbar{ true };
		bool mShowGizmoControls{ false };
		bool mShowResolutionControls{ false };
		bool mAllowZoom{ true };
		bool mAllowPan{ true };
		float mTopToolbarHeight{ 64.0f };
		float mBottomToolbarHeight{ 64.0f };
		glm::vec2 mPosition{ 0.0f, 0.0f };
		glm::vec2 mSize{ 0.0f, 0.0f };
		float mZoom{ 1.0f };
		bool mMouseInViewport{ false };
	};
}