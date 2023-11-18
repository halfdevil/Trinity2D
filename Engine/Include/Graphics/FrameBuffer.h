#pragma once

#include "Graphics/RenderTarget.h"
#include "Graphics/Texture.h"

namespace Trinity
{
    class FrameBuffer : public RenderTarget
    {
    public:

        FrameBuffer() = default;
        virtual ~FrameBuffer();

        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator = (const FrameBuffer&) = delete;

        FrameBuffer(FrameBuffer&&) = default;
        FrameBuffer& operator = (FrameBuffer&&) = default;

        uint32_t getWidth() const
        {
            return mWidth;
        }

        uint32_t getHeight() const
        {
            return mHeight;
        }

        virtual bool create(uint32_t width, uint32_t height);
        virtual void destroy();

        virtual std::vector<Texture*> getColorTextures() const;
        virtual Texture* getColorTexture(uint32_t index = 0) const;
        virtual Texture* getDepthTexture() const;

        virtual std::vector<wgpu::TextureFormat> getColorFormats() const override;
        virtual wgpu::TextureFormat getColorFormat(uint32_t index = 0) const override;
        virtual wgpu::TextureFormat getDepthFormat() const override;

        virtual bool hasDepthStencilAttachment() const override;
        virtual std::vector<wgpu::RenderPassColorAttachment> getColorAttachments() const override;
        virtual wgpu::RenderPassDepthStencilAttachment getDepthStencilAttachment() const override;
        virtual void resize(uint32_t width, uint32_t height) override;

        virtual bool addColorAttachment(
            wgpu::TextureFormat colorFormat,
            wgpu::TextureUsage usage,
            wgpu::Color clearColor = { 0, 0, 0, 1 },
            wgpu::LoadOp loadOp = wgpu::LoadOp::Clear,
            wgpu::StoreOp storeOp = wgpu::StoreOp::Store
        );

        virtual bool setDepthStencilAttachment(
            wgpu::TextureFormat depthFormat,
            wgpu::TextureUsage usage,
            float depthValue = 1.0f,
            wgpu::LoadOp depthLoadOp = wgpu::LoadOp::Clear,
            wgpu::StoreOp depthStoreOp = wgpu::StoreOp::Store,
            uint32_t stencilValue = 0,
            wgpu::LoadOp stencilLoadOp = wgpu::LoadOp::Undefined,
            wgpu::StoreOp stencilStoreOp = wgpu::StoreOp::Undefined
        );

    private:

        uint32_t mWidth{ 0 };
        uint32_t mHeight{ 0 };
        std::vector<std::unique_ptr<Texture>> mColorTextures;
        std::unique_ptr<Texture> mDepthTexture{ nullptr };
        std::vector<wgpu::RenderPassColorAttachment> mColorAttachments;
        wgpu::RenderPassDepthStencilAttachment mDepthStencilAttachment;
    };
}