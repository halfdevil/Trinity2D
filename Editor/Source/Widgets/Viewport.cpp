#include "Widgets/Viewport.h"
#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/TextureRenderable.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderPass.h"
#include "Core/EditorTheme.h"
#include "Core/EditorResources.h"
#include "Core/EditorGizmo.h"
#include "Core/EditorCamera.h"
#include "Core/EditorGrid.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "Input/Input.h"
#include "Utils/EditorHelper.h"
#include "glm/gtc/type_ptr.hpp"
#include "IconsFontAwesome6.h"

namespace Trinity
{
	Viewport::~Viewport()
	{
		destroy();
	}

	bool Viewport::create(EditorResources& resources, uint32_t resolutionIndex,	const glm::vec4& clearColor)
	{
		mResolutions = {
			{ "1920 x 1080", 1920, 1080 },
			{ "1280 x 720", 1280, 720 },
			{ "1024 x 768", 1024, 768 },
			{ "720 x 480", 720, 480 },
			{ "640 x 480", 640, 480 },
		};

		mSelectedResolution = &mResolutions[resolutionIndex];
		mFrameBuffer = std::make_unique<FrameBuffer>();

		if (!mFrameBuffer->create(mSelectedResolution->width, mSelectedResolution->height))
		{
			LogError("FrameBuffer::create() failed (%dx%d)", mSelectedResolution->width, mSelectedResolution->height);
			return false;
		}

		if (!mFrameBuffer->addColorAttachment(
			wgpu::TextureFormat::RGBA8Unorm, 
			wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding,
			{ clearColor.r, clearColor.g, clearColor.b, clearColor.a }
		))
		{
			LogError("FrameBuffer::addColorAttachment() failed");
			return false;
		}

		if (!mFrameBuffer->setDepthStencilAttachment(
			wgpu::TextureFormat::Depth32Float,
			wgpu::TextureUsage::RenderAttachment
		))
		{
			LogError("FrameBuffer::setDepthStencilAttachment() failed");
			return false;
		}

		mGrid = std::make_unique<EditorGrid>();
		if (!mGrid->create(*mFrameBuffer, *resources.getResourceCache()))
		{
			LogError("EditorGrid::create() failed");
			return false;
		}

		auto& input = Input::get();
		auto* mouse = input.getMouse();

		mouse->onPositionUpdated.subscribe([this](auto x, auto y) {
			onMousePositionUpdated(x, y);
		});

		mouse->onScrollUpdated.subscribe([this](auto x, auto y) {
			onMouseScrollUpdated(x, y);
		});

		mouse->onButtonStateUpdated.subscribe([this](auto button, auto pressed) {
			onMouseButtonStateUpdated(button, pressed);
		});

		mRenderPass = std::make_unique<RenderPass>();
		mGizmo = std::make_unique<EditorGizmo>();
		mCamera = std::make_unique<EditorCamera>();

		return true;
	}

	void Viewport::destroy()
	{
		mFrameBuffer = nullptr;
	}

	void Viewport::resize(uint32_t width, uint32_t height)
	{
		if (mFrameBuffer != nullptr)
		{
			mFrameBuffer->resize(width, height);
		}

		onViewportResize(width, height);
	}

	void Viewport::drawContent(float deltaTime)
	{

	}

	void Viewport::update(float deltaTime)
	{
		if (mCamera != nullptr)
		{
			mCamera->update(deltaTime);
		}
	}

	void Viewport::draw()
	{
		if (!isEnabled())
		{
			return;
		}

		ImGui::Begin(mTitle.c_str(), &mEnabled);
		{
			mActive = ImGui::IsWindowFocused();

			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 framePadding = ImGui::GetStyle().FramePadding;
			ImVec2 cMin = ImGui::GetWindowContentRegionMin();
			ImVec2 cMax = ImGui::GetWindowContentRegionMax();
			ImVec2 size = { cMax.x - cMin.x, cMax.y - cMin.y };

			if (mShowToolbar)
			{
				drawToolbar(
					windowPos.x + cMin.x, 
					windowPos.y + cMin.y, 
					cMax.x, 
					mToolbarHeight
				);
				
				size.y -= (mToolbarHeight + framePadding.y * 2.0f);
			}

			if (mShowBottomPanel)
			{
				size.y -= (mBottomPanelHeight + framePadding.y * 2.0f);
			}

			if (mFrameBuffer != nullptr)
			{
				auto* texture = mFrameBuffer->getColorTexture();
				if (texture != nullptr)
				{
					auto& theme = EditorTheme::get();

					float width = (float)texture->getWidth() * theme.getScaleFactor();
					float height = (float)texture->getHeight() * theme.getScaleFactor();
					float aspect = width / height;

					if (size.y < height)
					{
						height = size.y;
						width = aspect * height;
					}

					if (size.x < width)
					{
						width = size.x;
						height = width / aspect;
					}

					ImVec2 position = {
						ImGui::GetCursorPosX() + (size.x / 2 - width / 2),
						ImGui::GetCursorPosY() + (size.y / 2 - height / 2)
					};

					ImGui::SetCursorPos(position);
					ImGui::Image(texture, { width, height });

					mPosition = { windowPos.x + position.x, windowPos.y + position.y };
					mSize = { width, height };

					editTransform(mPosition.x, mPosition.y, mSize.x, mSize.y);
				}
			}

			if (mShowBottomPanel)
			{
				drawBottomPanel(
					windowPos.x + cMin.x,
					windowPos.y + cMax.y - mBottomPanelHeight,
					cMax.x, mBottomPanelHeight
				);
			}

			ImGui::End();
		}
	}

	void Viewport::drawToolbar(float x, float y, float width, float height)
	{
		ImGuiWindowFlags windowFlags = 0
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoSavedSettings;

		ImGui::SetNextWindowPos({ x, y });
		ImGui::BeginChild("##toolbar", ImVec2{ width, height }, false, windowFlags);
		{
			auto operation = mGizmo->getOperation();

			if (EditorHelper::toggleButton(ICON_FA_UP_DOWN_LEFT_RIGHT, operation == GizmoOperation::Translate))
			{
				mGizmo->setOperation(GizmoOperation::Translate);
			}

			ImGui::SameLine();

			if (EditorHelper::toggleButton(ICON_FA_ROTATE, operation == GizmoOperation::Rotate))
			{
				mGizmo->setOperation(GizmoOperation::Rotate);
			}

			ImGui::SameLine();

			if (EditorHelper::toggleButton(ICON_FA_MAXIMIZE, operation == GizmoOperation::Scale))
			{
				mGizmo->setOperation(GizmoOperation::Scale);
			}

			ImGui::SameLine();

			if (ImGui::BeginCombo("##resolution", mSelectedResolution->name.c_str(), ImGuiComboFlags_WidthFitPreview))
			{
				for (auto& resolution : mResolutions)
				{
					bool isSelected = mSelectedResolution == &resolution;
					if (ImGui::Selectable(resolution.name.c_str(), isSelected))
					{
						mSelectedResolution = &resolution;
						resize(mSelectedResolution->width, mSelectedResolution->height);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			ImGui::Separator();
			ImGui::EndChild();
		}
	}

	void Viewport::drawBottomPanel(float x, float y, float width, float height)
	{
		
	}

	void Viewport::editTransform(float x, float y, float width, float height)
	{		
	}

	void Viewport::onViewportResize(uint32_t width, uint32_t height)
	{
		onResize.notify(width, height);

		if (mCamera != nullptr)
		{
			mCamera->setSize(0.0f, (float)width, 0.0f, (float)height);
			mCamera->setPosition(glm::vec2{ 0.0f });
			mCamera->setRotation(0.0f);
		}

		if (mGrid != nullptr)
		{
			mGrid->updateGridData({
				.resolution = glm::vec2{ (float)width, (float)height },
				.scale = glm::vec2{ 0.02f, 0.08f }
			});
		}
	}

	void Viewport::onMouseButtonStateUpdated(int32_t button, bool pressed)
	{
		if (button == MOUSE_BUTTON_2)
		{
			mIsRightButtonDown = pressed;
		}
	}

	void Viewport::onMousePositionUpdated(float x, float y)
	{
		mMousePosition = { x, y };

		if (mActive)
		{
			if ((mMousePosition.x >= mPosition.x && mMousePosition.x < mPosition.x + mSize.x) &&
				(mMousePosition.y >= mPosition.y && mMousePosition.y < mPosition.y + mSize.y))
			{
				if (mCamera != nullptr && mIsRightButtonDown)
				{
					const auto& cameraSize = mCamera->getSize();

					glm::vec2 scale{
						cameraSize.x / mSize.x,
						cameraSize.y / mSize.y
					};

					mCamera->panBy({
						(mOldMousePosition.x - mMousePosition.x) * scale.x,
						(mMousePosition.y - mOldMousePosition.y) * scale.y
					});
				}
			}
		}

		mOldMousePosition = mMousePosition;
	}

	void Viewport::onMouseScrollUpdated(float x, float y)
	{
		if (mActive)
		{
			if ((mMousePosition.x < mPosition.x || mMousePosition.x >= mPosition.x + mSize.x) ||
				(mMousePosition.y < mPosition.y || mMousePosition.y >= mPosition.y + mSize.y))
			{
				return;
			}

			if (mCamera != nullptr)
			{
				mCamera->zoomBy(1.0f + (kZoomSpeed * y * -1.0f));
			}
		}
	}
}