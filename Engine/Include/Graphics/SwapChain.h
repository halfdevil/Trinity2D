#pragma once

#include "Graphics/RenderTarget.h"
#include <string>

namespace Trinity
{
    class SwapChain : public RenderTarget
    {
    public:

        SwapChain() = default;
        ~SwapChain();

        SwapChain(const SwapChain&) = delete;
        SwapChain& operator = (const SwapChain&) = delete;

        SwapChain(SwapChain&&) = default;
        SwapChain& operator = (SwapChain&&) = default;

        const wgpu::SwapChain& getHandle() const
        {
            return mHandle;
        }

        const wgpu::Texture& getDepthStencilTexture() const
        {
            return mDepthStencilTexture;
        }

        const wgpu::TextureView& getDepthStencilView() const
        {
            return mDepthStencilView;
        }

        wgpu::TextureView getCurrentView() const
        {
            return mHandle.GetCurrentTextureView();
        }

        uint32_t getWidth() const
        {
            return mWidth;
        }

        uint32_t getHeight() const
        {
            return mHeight;
        }

        const wgpu::Color& getClearColor() const
        {
            return mClearColor;
        }

        wgpu::TextureFormat getColorFormat() const
        {
            return mColorFormat;
        }

        operator const wgpu::SwapChain& () const
        {
            return mHandle;
        }

        virtual bool create(
            uint32_t width, 
            uint32_t height, 
            const wgpu::Surface& surface,
            wgpu::PresentMode presentMode, 
            wgpu::TextureFormat colorFormat, 
            wgpu::TextureFormat depthFormat
        );

		virtual void destroy();
        virtual void setClearColor(const wgpu::Color& clearColor);
        virtual void present();

        virtual std::vector<wgpu::TextureFormat> getColorFormats() const override;
        virtual wgpu::TextureFormat getColorFormat(uint32_t index) const override;
        virtual wgpu::TextureFormat getDepthFormat() const override;

        virtual bool hasDepthStencilAttachment() const override;
        virtual std::vector<wgpu::RenderPassColorAttachment> getColorAttachments() const override;
        virtual wgpu::RenderPassDepthStencilAttachment getDepthStencilAttachment() const override;

    private:

        uint32_t mWidth{ 0 };
        uint32_t mHeight{ 0 };
        wgpu::SwapChain mHandle;
        wgpu::Texture mDepthStencilTexture;
        wgpu::TextureView mDepthStencilView;
        wgpu::Color mClearColor;
        wgpu::TextureFormat mColorFormat;
        wgpu::TextureFormat mDepthFormat;
    };
}