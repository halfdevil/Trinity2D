#include "Graphics/FrameBuffer.h"
#include "Core/Debugger.h"
#include "Core/Logger.h"

namespace Trinity
{
    std::vector<wgpu::TextureFormat> FrameBuffer::getColorFormats() const
    {
        return mColorFormats;
    }

    wgpu::TextureFormat FrameBuffer::getDepthFormat() const
    {
        return mDepthFormat;
    }

    bool FrameBuffer::hasDepthStencilAttachment() const
    {
        return mDepthFormat != wgpu::TextureFormat::Undefined;
    }

    std::vector<wgpu::RenderPassColorAttachment> FrameBuffer::getColorAttachments() const
    {
        return mColorAttachments;
    }

    wgpu::RenderPassDepthStencilAttachment FrameBuffer::getDepthStencilAttachment() const
    {
        return mDepthStencilAttachment;
    }

    void FrameBuffer::addColorAttachment(const Texture& texture, wgpu::Color clearColor, 
        wgpu::LoadOp loadOp, wgpu::StoreOp storeOp)
    {
        mColorFormats.push_back(texture.getFormat());
        mColorAttachments.push_back({
            .view = texture.getView(),
            .loadOp = loadOp,
            .storeOp = storeOp,
            .clearValue = clearColor
        });
    }

    void FrameBuffer::setDepthStencilAttachment(const Texture& texture, float depthValue, 
        wgpu::LoadOp loadOp, wgpu::StoreOp storeOp)
    {
        mDepthFormat = texture.getFormat();
        mDepthStencilAttachment = {
            .view = texture.getView(),
            .depthLoadOp = loadOp,
            .depthStoreOp = storeOp,
            .depthClearValue = depthValue
        };
    }
}