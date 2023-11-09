#include "Graphics/VertexLayout.h"

namespace Trinity
{
    VertexLayout::VertexLayout(std::vector<wgpu::VertexAttribute> attributes,
        wgpu::VertexStepMode stepMode)
    {
        setAttributes(std::move(attributes), stepMode);
    }

    std::type_index VertexLayout::getType() const
    {
        return typeid(VertexLayout);
    }

    void VertexLayout::setAttributes(std::vector<wgpu::VertexAttribute> attributes,
        wgpu::VertexStepMode stepMode)
    {
        mAttributes = std::move(attributes);
        mSize = 0;

        for (const wgpu::VertexAttribute& attribute : mAttributes)
        {
            switch (attribute.format)
            {
            case wgpu::VertexFormat::Float16x2:
                mSize += 4;
                break;

            case wgpu::VertexFormat::Float16x4:
                mSize += 8;
                break;

            case wgpu::VertexFormat::Float32:
                mSize += 4;
                break;

            case wgpu::VertexFormat::Float32x2:
                mSize += 8;
                break;

            case wgpu::VertexFormat::Float32x3:
                mSize += 12;
                break;

            case wgpu::VertexFormat::Float32x4:
                mSize += 16;
                break;

            case wgpu::VertexFormat::Uint32x4:
                mSize += 16;
                break;

            case wgpu::VertexFormat::Unorm8x4:
                mSize += 4;

            default:
                break;
            }
        }

        mBufferLayout = {
            .arrayStride = mSize,
            .stepMode = stepMode,
            .attributeCount = (uint32_t)mAttributes.size(),
            .attributes = mAttributes.data()
        };
    }
}