#include "Widgets/Viewport.h"
#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/TextureRenderable.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderPass.h"
#include "Core/EditorTheme.h"
#include "Core/EditorResources.h"
#include "Core/EditorGizmo.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "Utils/EditorHelper.h"
#include "glm/gtc/type_ptr.hpp"
#include "IconsFontAwesome6.h"

namespace Trinity
{
	Viewport::~Viewport()
	{
		destroy();
	}

	bool Viewport::create(EditorResources& resources)
	{
		mResolutions = {
			{ "1920 x 1080", 1920, 1080 },
			{ "1280 x 720", 1280, 720 },
			{ "1024 x 768", 1024, 768 },
			{ "720 x 480", 720, 480 },
			{ "640 x 480", 640, 480 },
		};

		mSelectedResolution = &mResolutions[0];
		mFrameBuffer = std::make_unique<FrameBuffer>();

		if (!mFrameBuffer->create(mSelectedResolution->width, mSelectedResolution->height))
		{
			LogError("FrameBuffer::create() failed (%dx%d)", mSelectedResolution->width, mSelectedResolution->height);
			return false;
		}

		if (!mFrameBuffer->addColorAttachment(
			wgpu::TextureFormat::RGBA8Unorm, 
			wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding,
			{ 0.5f, 0.5f, 0.5f, 1.0f }
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

		mRenderPass = std::make_unique<RenderPass>();
		mGizmo = std::make_unique<EditorGizmo>();

		return true;
	}

	void Viewport::destroy()
	{
		mFrameBuffer = nullptr;
	}

	void Viewport::setCamera(Camera& camera)
	{
		mCamera = &camera;
		mGizmo->setCamera(camera);
	}

	void Viewport::resize(uint32_t width, uint32_t height)
	{
		if (mFrameBuffer != nullptr)
		{
			mFrameBuffer->resize(width, height);
		}

		onResize.notify(width, height);
	}

	void Viewport::drawContent(float deltaTime)
	{

	}

	void Viewport::draw()
	{
		if (!isEnabled())
		{
			return;
		}

		ImGui::Begin(mTitle.c_str(), &mEnabled);
		{
			if (mFrameBuffer != nullptr)
			{
				auto* texture = mFrameBuffer->getColorTexture();
				if (texture != nullptr)
				{
					auto& theme = EditorTheme::get();

					float width = (float)texture->getWidth() * theme.getScaleFactor();
					float height = (float)texture->getHeight() * theme.getScaleFactor();
					float aspect = width / height;

					ImVec2 cMin = ImGui::GetWindowContentRegionMin();
					ImVec2 cMax = ImGui::GetWindowContentRegionMax();
					ImVec2 size = { cMax.x - cMin.x, cMax.y - cMin.y };

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

					if (mCamera != nullptr)
					{
						ImVec2 windowPos = ImGui::GetWindowPos();
						editTransform(windowPos.x + position.x, windowPos.y + position.y, width, height);
					}
				}
			}

			auto windowPos = ImGui::GetWindowPos();
			auto contentMin = ImGui::GetWindowContentRegionMin();
			drawControls(contentMin.x, contentMin.y);

			ImGui::End();
		}
	}

	void Viewport::drawControls(float x, float y)
	{
		ImGui::SetCursorPos({ x, y });
		ImGui::BeginGroup();
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

			ImGui::EndGroup();
		}
	}

	void Viewport::editTransform(float x, float y, float width, float height)
	{		
	}
}