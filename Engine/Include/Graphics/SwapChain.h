#pragma once

#include "Core/Resource.h"
#include <webgpu/webgpu_cpp.h>
#include <string>

namespace Trinity
{
    class SwapChain : public Resource
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

        bool hasDepthStencilAttachment() const
        {
            return mDepthStencilView != nullptr;
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

        wgpu::TextureFormat getDepthFormat() const
        {
            return mDepthFormat;
        }

        operator const wgpu::SwapChain& () const
        {
            return mHandle;
        }

        virtual bool create(uint32_t width, uint32_t height, const wgpu::Surface& surface,
            wgpu::PresentMode presentMode, wgpu::TextureFormat depthFormat);

		virtual void destroy();
        virtual void setClearColor(const wgpu::Color& clearColor);
        virtual void present();

		virtual std::type_index getType() const override;

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