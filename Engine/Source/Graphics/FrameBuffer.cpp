#include "Graphics/FrameBuffer.h"
#include "Core/Debugger.h"
#include "Core/Logger.h"
#include <algorithm>

namespace Trinity
{
    FrameBuffer::~FrameBuffer()
    {
        destroy();
    }

    bool FrameBuffer::create(uint32_t width, uint32_t height)
    {
        mWidth = width;
        mHeight = height;

        return true;
    }

    void FrameBuffer::destroy()
    {
        mColorTextures.clear();
        mColorAttachments.clear();
        mDepthTexture = nullptr;
    }

    std::vector<Texture*> FrameBuffer::getColorTextures() const
    {
        std::vector<Texture*> textures(mColorTextures.size());
        std::transform(mColorTextures.begin(), mColorTextures.end(), textures.begin(),
            [](const auto& texture) {
                return dynamic_cast<Texture*>(texture.get());
            }
        );

        return textures;
    }

    Texture* FrameBuffer::getColorTexture(uint32_t index) const
    {
        if (index < (uint32_t)mColorTextures.size())
        {
            return mColorTextures[index].get();
        }

        return nullptr;
    }

    Texture* FrameBuffer::getDepthTexture() const
    {
        return mDepthTexture.get();
    }

    std::vector<wgpu::TextureFormat> FrameBuffer::getColorFormats() const
    {
        std::vector<wgpu::TextureFormat> formats(mColorTextures.size());
        std::transform(mColorTextures.begin(), mColorTextures.end(), formats.begin(),
            [](const auto& texture) {
                return texture->getFormat();
            }
        );

        return formats;
    }

    wgpu::TextureFormat FrameBuffer::getColorFormat(uint32_t index) const
    {
        if (index < (uint32_t)mColorTextures.size())
        {
            return mColorTextures.at(index)->getFormat();
        }

        return wgpu::TextureFormat::Undefined;
    }

    wgpu::TextureFormat FrameBuffer::getDepthFormat() const
    {
        if (mDepthTexture != nullptr)
        {
            return mDepthTexture->getFormat();
        }

        return wgpu::TextureFormat::Undefined;
    }

    bool FrameBuffer::hasDepthStencilAttachment() const
    {
        return mDepthTexture != nullptr;
    }

    std::vector<wgpu::RenderPassColorAttachment> FrameBuffer::getColorAttachments() const
    {
        return mColorAttachments;
    }

    wgpu::RenderPassDepthStencilAttachment FrameBuffer::getDepthStencilAttachment() const
    {
        return mDepthStencilAttachment;
    }

    void FrameBuffer::resize(uint32_t width, uint32_t height)
    {
        for (uint32_t idx = 0; idx < mColorTextures.size(); idx++)
        {
            auto& texture = mColorTextures[idx];
            auto& attachment = mColorAttachments[idx];

			if (!texture->create(width, height, texture->getFormat(), texture->getUsage()))
			{
				LogWarning("Texture::create() failed on resize (%dx%d)", width, height);
                return;
			}

            attachment.view = texture->getView();
        }

        if (mDepthTexture != nullptr)
        {
            if (!mDepthTexture->create(width, height, mDepthTexture->getFormat(), mDepthTexture->getUsage()))
            {
                LogWarning("Texture::create() failed on resize (%dx%d)", width, height);
                return;
            }

            mDepthStencilAttachment.view = mDepthTexture->getView();
        }

        mWidth = width;
        mHeight = height;
    }

    bool FrameBuffer::addColorAttachment(
        wgpu::TextureFormat colorFormat, 
        wgpu::TextureUsage usage, 
        wgpu::Color clearColor, 
        wgpu::LoadOp loadOp, 
        wgpu::StoreOp storeOp
    )
    {
        auto texture = std::make_unique<Texture>();
        if (!texture->create(mWidth, mHeight, colorFormat, usage))
        {
            LogError("Texture::create() failed for FrameBuffer");
            return false;
        }

        mColorAttachments.push_back({
            .view = texture->getView(),
            .loadOp = loadOp,
            .storeOp = storeOp,
            .clearValue = clearColor
        });

        mColorTextures.push_back(std::move(texture));
        return true;
    }

    bool FrameBuffer::setDepthStencilAttachment(
        wgpu::TextureFormat depthFormat, 
        wgpu::TextureUsage usage, 
        float depthValue, 
        wgpu::LoadOp depthLoadOp, 
        wgpu::StoreOp depthStoreOp, 
        uint32_t stencilValue, 
        wgpu::LoadOp stencilLoadOp, 
        wgpu::StoreOp stencilStoreOp
    )
    {
        mDepthTexture = std::make_unique<Texture>();
        if (!mDepthTexture->create(mWidth, mHeight, depthFormat, usage))
        {
            LogError("Texture::create() failed for FrameBuffer depth texture");
            return false;
        }

        mDepthStencilAttachment = {
            .view = mDepthTexture->getView(),
            .depthLoadOp = depthLoadOp,
            .depthStoreOp = depthStoreOp,
            .depthClearValue = depthValue,
            .stencilLoadOp = stencilLoadOp,
            .stencilStoreOp = stencilStoreOp,
            .stencilClearValue = stencilValue
        };

        return true;
    }
}