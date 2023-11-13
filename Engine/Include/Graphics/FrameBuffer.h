#pragma once

#include "Graphics/RenderTarget.h"
#include "Graphics/Texture.h"

namespace Trinity
{
    class FrameBuffer : public RenderTarget
    {
    public:

        FrameBuffer() = default;
        ~FrameBuffer() = default;

        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator = (const FrameBuffer&) = delete;

        FrameBuffer(FrameBuffer&&) = default;
        FrameBuffer& operator = (FrameBuffer&&) = default;

        virtual std::vector<wgpu::TextureFormat> getColorFormats() const override;
        virtual wgpu::TextureFormat getDepthFormat() const override;

        virtual bool hasDepthStencilAttachment() const override;
        virtual std::vector<wgpu::RenderPassColorAttachment> getColorAttachments() const override;
        virtual wgpu::RenderPassDepthStencilAttachment getDepthStencilAttachment() const override;

        void addColorAttachment(
            const Texture& texture, 
            wgpu::Color clearColor = { 0, 0, 0, 1 },
            wgpu::LoadOp loadOp = wgpu::LoadOp::Clear,
            wgpu::StoreOp storeOp = wgpu::StoreOp::Store
        );

        void setDepthStencilAttachment(const Texture& texture, 
            float depthValue = 0.0f,
            wgpu::LoadOp loadOp = wgpu::LoadOp::Clear,
            wgpu::StoreOp storeOp = wgpu::StoreOp::Store            
        );

    private:

        std::vector<wgpu::RenderPassColorAttachment> mColorAttachments;
        std::vector<wgpu::TextureFormat> mColorFormats;
        wgpu::RenderPassDepthStencilAttachment mDepthStencilAttachment;
        wgpu::TextureFormat mDepthFormat{ wgpu::TextureFormat::Undefined };
    };
}