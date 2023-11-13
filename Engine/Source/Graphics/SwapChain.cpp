#include "Graphics/SwapChain.h"
#include "Graphics/GraphicsDevice.h"
#include "Core/Debugger.h"
#include "Core/Logger.h"

namespace Trinity
{
    SwapChain::~SwapChain()
    {
        destroy();
    }

    bool SwapChain::create(
        uint32_t width, 
        uint32_t height, 
        const wgpu::Surface& surface,
        wgpu::PresentMode presentMode, 
        wgpu::TextureFormat colorFormat, 
        wgpu::TextureFormat depthFormat)
    {
        mWidth = width;
        mHeight = height;
        mColorFormat = colorFormat;
        mDepthFormat = depthFormat;

        const wgpu::Device& device = GraphicsDevice::get();

        wgpu::SwapChainDescriptor swapChainDesc = {
            .label = "SwapChain.Target",
            .usage = wgpu::TextureUsage::RenderAttachment,
            .format = mColorFormat,
            .width = mWidth,
            .height = mHeight,
            .presentMode = presentMode
        };

        mHandle = device.CreateSwapChain(surface, &swapChainDesc);
        if (!mHandle)
        {
            LogError("wgpu::Device::CreateSwapChain() failed!!");
            return false;
        }

        if (mDepthFormat != wgpu::TextureFormat::Undefined)
        {
            wgpu::TextureDescriptor textureDesc = {
                .label = "SwapChain.DepthTexture",
                .usage = wgpu::TextureUsage::RenderAttachment,
                .size = {
                    .width = mWidth,
                    .height = mHeight,
                    .depthOrArrayLayers = 1
                },
                .format = mDepthFormat
            };

            mDepthStencilTexture = device.CreateTexture(&textureDesc);
            if (!mDepthStencilTexture)
            {
                LogError("wgpu::Device::CreateTexture() failed!!");
                return false;
            }

            mDepthStencilView = mDepthStencilTexture.CreateView();
            if (!mDepthStencilView)
            {
                LogError("wgpu::Texture::CreateView() failed!!");
                return false;
            }
        }

        return true;
    }

    void SwapChain::destroy()
    {
        mHandle = nullptr;
        mDepthStencilTexture = nullptr;
    }

    void SwapChain::setClearColor(const wgpu::Color& clearColor)
    {
        mClearColor = clearColor;
    }

    void SwapChain::present()
    {
#ifndef __EMSCRIPTEN__
        mHandle.Present();
#endif
    }

    std::vector<wgpu::TextureFormat> SwapChain::getColorFormats() const
    {
        return { mColorFormat };
    }

    wgpu::TextureFormat SwapChain::getColorFormat(uint32_t index) const
    {
        Assert(index == 0, "Invalid index passed: %ld", index);
        return mColorFormat;
    }

    wgpu::TextureFormat SwapChain::getDepthFormat() const
    {
        return mDepthFormat;
    }

    bool SwapChain::hasDepthStencilAttachment() const
    {
        return mDepthFormat != wgpu::TextureFormat::Undefined;
    }

    std::vector<wgpu::RenderPassColorAttachment> SwapChain::getColorAttachments() const
    {
        return { {
            .view = mHandle.GetCurrentTextureView(),
            .loadOp = wgpu::LoadOp::Clear,
            .storeOp = wgpu::StoreOp::Store,
            .clearValue = mClearColor
        } };
    }

    wgpu::RenderPassDepthStencilAttachment SwapChain::getDepthStencilAttachment() const
    {
        return {
            .view = mDepthStencilView,
            .depthLoadOp = wgpu::LoadOp::Clear,
            .depthStoreOp = wgpu::StoreOp::Store,
            .depthClearValue = 1.0f
        };
    }
}