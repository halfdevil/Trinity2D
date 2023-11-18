#pragma once

#include "Core/Resource.h"
#include <webgpu/webgpu_cpp.h>

namespace Trinity
{
    class Image;

    class Texture : public Resource
    {
    public:

        Texture() = default;
        virtual ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator = (const Texture&) = delete;

        Texture(Texture&&) = default;
        Texture& operator = (Texture&&) = default;

        const wgpu::Texture& getHandle() const
        {
            return mHandle;
        }

        const wgpu::TextureView& getView() const
        {
            return mView;
        }

        wgpu::TextureFormat getFormat() const
        {
            return mFormat;
        }

        wgpu::TextureUsage getUsage() const
        {
            return mUsage;
        }

        uint32_t getWidth() const
        {
            return mWidth;
        }

        uint32_t getHeight() const
        {
            return mHeight;
        }

        bool hasMipMaps() const
        {
            return mHasMipmaps;
        }

		virtual bool create(uint32_t width, uint32_t height, wgpu::TextureFormat format, wgpu::TextureUsage usage);
        virtual bool create(const std::string& fileName, wgpu::TextureFormat format, bool mipmaps = false);
		virtual bool create(Image* image, wgpu::TextureFormat format, bool mipmaps = false);
		virtual void destroy();

		virtual void upload(uint32_t channels, const void* data, uint32_t size);
		virtual void upload(uint32_t level, uint32_t width, uint32_t height, uint32_t channels,
			const void* data, uint32_t size);

        virtual std::type_index getType() const override;

    protected:

        wgpu::TextureFormat mFormat{ wgpu::TextureFormat::RGBA8UnormSrgb };
        wgpu::TextureUsage mUsage{ wgpu::TextureUsage::None };
        wgpu::Texture mHandle;
		wgpu::TextureView mView;
		uint32_t mWidth{ 0 };
		uint32_t mHeight{ 0 };
		bool mHasMipmaps{ false };
    };
}