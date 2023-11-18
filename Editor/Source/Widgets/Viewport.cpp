#include "Widgets/Viewport.h"
#include "Graphics/Texture.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Core/EditorResources.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"

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
	}

	void Viewport::draw()
	{
		if (ImGui::Begin(mTitle.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar))
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
				}
			}

			ImGui::End();
		}
	}
}