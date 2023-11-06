#pragma once

#include "Core/Resource.h"
#include "Graphics/Texture.h"

namespace Trinity
{
    class FrameBuffer : public Resource
    {
    public:

        FrameBuffer() = default;
        ~FrameBuffer() = default;

        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator = (const FrameBuffer&) = delete;

        FrameBuffer(FrameBuffer&&) = default;
        FrameBuffer& operator = (FrameBuffer&&) = default;

        const std::vector<wgpu::RenderPassColorAttachment>& getColorAttachments() const
        {
            return mColorAttachments;
        }

        const wgpu::RenderPassDepthStencilAttachment& getDepthAttachment() const
        {
            return mDepthStencilAttachment;
        }

        bool hasDepthStencilAttachment() const
        {
            return mHasDepthStencilAttachment;
        }

        virtual std::type_index getType() const override;

        bool addColorAttachment(const Texture& texture, wgpu::Color clearColor = { 0, 0, 0, 1 });
        bool setDepthAttachment(const Texture& texture, float depthValue = 0.0f);

    private:

        std::vector<wgpu::RenderPassColorAttachment> mColorAttachments;
        wgpu::RenderPassDepthStencilAttachment mDepthStencilAttachment;
        bool mHasDepthStencilAttachment{ false };
    };
}