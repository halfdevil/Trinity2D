#pragma once

#include "Core/Resource.h"
#include "Graphics/Texture.h"

namespace Trinity
{
    class RenderTarget : public Resource
    {
    public:

		RenderTarget() = default;
		virtual ~RenderTarget() = 0;

		RenderTarget(const RenderTarget&) = delete;
		RenderTarget& operator = (const RenderTarget&) = delete;

		RenderTarget(RenderTarget&&) = default;
		RenderTarget& operator = (RenderTarget&&) = default;

		virtual std::vector<wgpu::TextureFormat> getColorFormats() const = 0;
		virtual wgpu::TextureFormat getColorFormat(uint32_t index = 0) const = 0;
		virtual wgpu::TextureFormat getDepthFormat() const = 0;

		virtual bool hasDepthStencilAttachment() const = 0;
		virtual std::vector<wgpu::RenderPassColorAttachment> getColorAttachments() const = 0;
		virtual wgpu::RenderPassDepthStencilAttachment getDepthStencilAttachment() const = 0;

		virtual std::type_index getType() const override;
    };
}