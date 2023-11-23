#include "Widgets/Viewport.h"
#include "Scene/Node.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/TextureRenderable.h"
#include "Graphics/Texture.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
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

	bool Viewport::create(uint32_t width, uint32_t height, EditorResources& resources)
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

		mGizmo = std::make_unique<EditorGizmo>();
		return true;
	}

	void Viewport::destroy()
	{
		mFrameBuffer = nullptr;
	}

	void Viewport::setSelectedNode(Node* node)
	{
		mSelectedNode = node;

		if (mSelectedNode != nullptr)
		{
			if (mSelectedNode->hasComponent<TextureRenderable>())
			{
				mRenderable = &mSelectedNode->getComponent<TextureRenderable>();
			}
			else
			{
				mRenderable = nullptr;
			}
		}
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

	void Viewport::drawGizmoControls(float x, float y)
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

	void Viewport::editTransform(float x, float y, float width, float height)
	{
		auto& transform = mSelectedNode->getTransform();
		auto matrix = transform.getWorldMatrix();

		mGizmo->setRect(x, y, width, height);

		glm::vec2 origin{ 0.0f };
		glm::vec2 size{ 0.0f };

		if (mRenderable != nullptr)
		{
			auto* texture = mRenderable->getTexture();
			
			origin = mRenderable->getOrigin();
			size = { (float)texture->getWidth(), (float)texture->getHeight() };
		}

		if (mGizmo->show(matrix, origin, size))
		{
			transform.setWorldMatrix(matrix);
		}
	}
}