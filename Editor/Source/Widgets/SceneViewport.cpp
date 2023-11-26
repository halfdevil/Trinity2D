#include "Widgets/SceneViewport.h"
#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "Scene/RenderSystem.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/TextureRenderable.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderPass.h"
#include "Core/EditorResources.h"
#include "Core/EditorGizmo.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "Utils/EditorHelper.h"
#include "glm/gtc/type_ptr.hpp"
#include "IconsFontAwesome6.h"

namespace Trinity
{
	SceneViewport::~SceneViewport()
	{
		destroy();
	}

	bool SceneViewport::create(uint32_t width, uint32_t height, EditorResources& resources)
	{
		mFrameBuffer = std::make_unique<FrameBuffer>();
		if (!mFrameBuffer->create(width, height))
		{
			LogError("FrameBuffer::create() failed (%dx%d)", width, height);
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

		mRenderSystem = std::make_unique<RenderSystem>();
		if (!mRenderSystem->create(*mFrameBuffer))
		{
			LogError("RenderSystem::create() failed!!");
			return false;
		}

		mRenderPass = std::make_unique<RenderPass>();
		mGizmo = std::make_unique<EditorGizmo>();

		return true;
	}

	void SceneViewport::destroy()
	{
		mFrameBuffer = nullptr;
	}

	void SceneViewport::setScene(Scene& scene)
	{
		mScene = &scene;
		mSelectedNode = mScene->getRoot();
		mRenderSystem->setScene(scene);
	}

	void SceneViewport::setCamera(Camera& camera)
	{
		mCamera = &camera;
		mRenderSystem->setCamera(camera);
		mGizmo->setCamera(camera);
	}

	void SceneViewport::setSelectedNode(Node* node)
	{
		mSelectedNode = node;
	}

	void SceneViewport::resize(uint32_t width, uint32_t height)
	{
		if (mFrameBuffer != nullptr)
		{
			mFrameBuffer->resize(width, height);
		}
	}

	void SceneViewport::drawScene(float deltaTime)
	{
		mRenderPass->begin(*mFrameBuffer);
		mRenderSystem->draw(*mRenderPass);
		mRenderPass->end();
	}

	void SceneViewport::draw()
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
					float width = (float)texture->getWidth();
					float height = (float)texture->getHeight();
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

					if (mSelectedNode != nullptr && mCamera != nullptr)
					{
						ImVec2 windowPos = ImGui::GetWindowPos();
						editTransform(windowPos.x + position.x, windowPos.y + position.y, width, height);
					}
				}
			}

			auto windowPos = ImGui::GetWindowPos();
			auto contentMin = ImGui::GetWindowContentRegionMin();
			drawGizmoControls(contentMin.x, contentMin.y);

			ImGui::End();
		}
	}

	void SceneViewport::drawGizmoControls(float x, float y)
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

			ImGui::EndGroup();
		}
	}

	void SceneViewport::editTransform(float x, float y, float width, float height)
	{
		mGizmo->setRect(x, y, width, height);

		auto& transform = mSelectedNode->getTransform();
		auto matrix = transform.getWorldMatrix();

		if (mGizmo->show(matrix))
		{
			transform.setWorldMatrix(matrix);
		}
	}
}