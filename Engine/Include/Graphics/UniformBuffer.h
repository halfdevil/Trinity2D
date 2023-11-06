#pragma once

#include "Graphics/Buffer.h"

namespace Trinity
{
    class UniformBuffer : public Buffer
    {
    public:

        UniformBuffer() = default;
        ~UniformBuffer();

        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer& operator = (const UniformBuffer&) = delete;

        UniformBuffer(UniformBuffer&&) noexcept = default;
        UniformBuffer& operator = (UniformBuffer&&) noexcept = default;

        uint32_t getSize() const
        {
            return mSize;
        }

        virtual bool create(uint32_t size, const void* data = nullptr);
        virtual void destroy();

    private:

        uint32_t mSize{ 0 };
    };
}