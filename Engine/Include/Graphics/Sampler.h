#pragma once

#include "Core/Resource.h"
#include "webgpu/webgpu_cpp.h"

namespace Trinity
{
    class FileReader;
    class FileWriter;

    struct SamplerProperties
    {
        wgpu::AddressMode addressModeU{ wgpu::AddressMode::ClampToEdge };
        wgpu::AddressMode addressModeV{ wgpu::AddressMode::ClampToEdge };
        wgpu::AddressMode addressModeW{ wgpu::AddressMode::ClampToEdge };
        wgpu::FilterMode magFilter{ wgpu::FilterMode::Nearest };
        wgpu::FilterMode minFilter{ wgpu::FilterMode::Nearest };
        wgpu::MipmapFilterMode mipmapFilter{ wgpu::MipmapFilterMode::Nearest };
        wgpu::CompareFunction compare{ wgpu::CompareFunction::Undefined };
    };

    class Sampler : public Resource
    {
    public:

        Sampler() = default;
        virtual ~Sampler();

        Sampler(const Sampler&) = delete;
        Sampler& operator = (const Sampler&) = delete;

        Sampler(Sampler&&) = default;
        Sampler& operator = (Sampler&&) = default;

        const wgpu::Sampler& getHandle() const
        {
            return mHandle;
        }

        bool isValid() const
        {
            return mHandle != nullptr;
        }

		virtual bool create(const SamplerProperties& samplerProps);
		virtual void destroy();

        virtual std::type_index getType() const override;

    protected:

        wgpu::Sampler mHandle;
    };
}