#include "Graphics/RenderPass.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/RenderTarget.h"
#include "Core/Debugger.h"
#include "Core/Logger.h"

namespace Trinity
{
    std::type_index RenderPass::getType() const
    {
        return typeid(RenderPass);
    }

    bool RenderPass::begin(const RenderTarget& renderTarget)
    {        
        auto colorAttachments = renderTarget.getColorAttachments();
        auto depthStencilAttachment = renderTarget.getDepthStencilAttachment();

        wgpu::RenderPassDescriptor renderPassDesc = {
            .colorAttachmentCount = (uint32_t)colorAttachments.size(),
            .colorAttachments = colorAttachments.data()
        };

        if (renderTarget.hasDepthStencilAttachment())
        {
            renderPassDesc.depthStencilAttachment = &depthStencilAttachment;
        }

        auto& graphicsDevice = GraphicsDevice::get();
        mCommandEncoder = graphicsDevice.getDevice().CreateCommandEncoder();

        if (!mCommandEncoder)
        {
            LogError("Device::CreateCommandEncoder() failed");
            return false;
        }

        mRenderPassEncoder = mCommandEncoder.BeginRenderPass(&renderPassDesc);
        if (!mRenderPassEncoder)
        {
            LogError("wgpu::CommandEncoder::BeginRenderPass() failed!!");
            return false;
        }

        return true;
    }

    void RenderPass::end()
    {
        Assert(mRenderPassEncoder != nullptr, "RenderPass::begin() not called!!");
        mRenderPassEncoder.End();

        auto commands = mCommandEncoder.Finish();
        auto& graphicsDevice = GraphicsDevice::get();

        graphicsDevice.getQueue().Submit(1, &commands);
    }

    void RenderPass::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const
    {
        Assert(mRenderPassEncoder != nullptr, "RenderPass::begin() not called!!");
        mRenderPassEncoder.Draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void RenderPass::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
        int32_t baseVertex, uint32_t firstInstance) const
    {
        Assert(mRenderPassEncoder != nullptr, "RenderPass::begin() not called!!");
        mRenderPassEncoder.DrawIndexed(indexCount, instanceCount, firstIndex, baseVertex, firstInstance);
    }

    void RenderPass::setBindGroup(uint32_t groupIndex, const BindGroup& bindGroup) const
    {
        Assert(mRenderPassEncoder != nullptr, "RenderPass::begin() not called!!");
        mRenderPassEncoder.SetBindGroup(groupIndex, bindGroup.getHandle());
    }

    void RenderPass::setPipeline(const RenderPipeline& pipeline) const
    {
        Assert(mRenderPassEncoder != nullptr, "RenderPass::begin() not called!!");
        mRenderPassEncoder.SetPipeline(pipeline.getHandle());
    }

    void RenderPass::setVertexBuffer(uint32_t slot, const VertexBuffer& vertexBuffer) const
    {
        Assert(mRenderPassEncoder != nullptr, "RenderPass::begin() not called!!");
        mRenderPassEncoder.SetVertexBuffer(slot, vertexBuffer.getHandle());
    }

    void RenderPass::setIndexBuffer(const IndexBuffer& indexBuffer) const
    {
        Assert(mRenderPassEncoder != nullptr, "RenderPass::begin() not called!!");
        mRenderPassEncoder.SetIndexBuffer(indexBuffer.getHandle(), indexBuffer.getIndexFormat());
    }

    void RenderPass::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) const
    {
        Assert(mRenderPassEncoder != nullptr, "RenderPass::begin() not called!!");
        mRenderPassEncoder.SetViewport(x, y, width, height, minDepth, maxDepth);
    }

    void RenderPass::setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
    {
        Assert(mRenderPassEncoder != nullptr, "RenderPass::begin() not called!!");
        mRenderPassEncoder.SetScissorRect(x, y, width, height);
    }

    void RenderPass::setBlendConstant(float r, float g, float b, float a) const
    {
        Assert(mRenderPassEncoder != nullptr, "RenderPass::begin() not called!!");

        wgpu::Color c = {
            .r = r,
            .g = g,
            .b = b,
            .a = a
        };

        mRenderPassEncoder.SetBlendConstant(&c);
    }
}