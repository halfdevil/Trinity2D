#pragma once

#include "Graphics/Buffer.h"

namespace Trinity
{
    class IndexBuffer : public Buffer
    {
    public:

        IndexBuffer() = default;
        ~IndexBuffer();

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator = (const IndexBuffer&) = delete;

        IndexBuffer(IndexBuffer&&) = default;
        IndexBuffer& operator = (IndexBuffer&&) = default;

        wgpu::IndexFormat getIndexFormat() const
        {
            return mIndexFormat;
        }

        uint32_t getIndexSize() const
        {
            return mIndexSize;
        }

        uint32_t getNumIndices() const
        {
            return mNumIndices;
        }

        virtual bool create(wgpu::IndexFormat indexFormat, uint32_t numIndices, const void* data = nullptr);
        virtual void destroy();

    private:

        wgpu::IndexFormat mIndexFormat{ wgpu::IndexFormat::Undefined };
        uint32_t mIndexSize{ 0 };
        uint32_t mNumIndices{ 0 };
    };
}