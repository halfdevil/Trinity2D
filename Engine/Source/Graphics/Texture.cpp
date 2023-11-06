#include "Graphics/Texture.h"
#include "Graphics/GraphicsDevice.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/Image.h"
#include "Core/Logger.h"

namespace Trinity
{
	void Texture::destroy()
	{
		if (mHandle)
		{
			mHandle = nullptr;
			mView = nullptr;
		}
	}
	
	bool Texture::create(uint32_t width, uint32_t height, wgpu::TextureFormat format, wgpu::TextureUsage usage)
	{
		const wgpu::Device& device = GraphicsDevice::get();

		mFormat = format;
		mWidth = width;
		mHeight = height;

		wgpu::Extent3D size = {
			.width = mWidth,
			.height = mHeight,
			.depthOrArrayLayers = 1
		};

		wgpu::TextureDescriptor textureDesc = {
			.usage = usage,
			.dimension = wgpu::TextureDimension::e2D,
			.size = size,
			.format = mFormat,
			.mipLevelCount = 1,
			.sampleCount = 1
		};

		mHandle = device.CreateTexture(&textureDesc);
		if (!mHandle)
		{
			LogError("wgpu::Device::CreateTexture() failed!!");
			return false;
		}

		wgpu::TextureViewDescriptor textureViewDesc = {
			.format = mFormat,
			.dimension = wgpu::TextureViewDimension::e2D,
			.baseMipLevel = 0,
			.mipLevelCount = 1,
			.baseArrayLayer = 0,
			.arrayLayerCount = 1
		};

		mView = mHandle.CreateView(&textureViewDesc);
		if (!mView)
		{
			LogError("wgpu::Texture::CreateView() failed!!");
			return false;
		}

		return true;
	}

	bool Texture::create(Image* image, wgpu::TextureFormat format, bool hasMipmaps)
	{
		const wgpu::Device& device = GraphicsDevice::get();
		const wgpu::Queue& queue = GraphicsDevice::get().getQueue();

		mFormat = format;
		mHasMipmaps = hasMipmaps;
		mWidth = image->getWidth();
		mHeight = image->getHeight();

		std::vector<Mipmap> mipmaps{};
		if (hasMipmaps)
		{
			mipmaps = image->generateMipmaps();
		}

		wgpu::Extent3D size = {
			.width = mWidth,
			.height = mHeight,
			.depthOrArrayLayers = 1
		};

		wgpu::TextureDescriptor textureDesc = {
			.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
			.dimension = wgpu::TextureDimension::e2D,
			.size = size,
			.format = mFormat,
			.mipLevelCount = mHasMipmaps ? (uint32_t)mipmaps.size() : 1,
			.sampleCount = 1
		};

		mHandle = device.CreateTexture(&textureDesc);
		if (!mHandle)
		{
			LogError("wgpu::Device::CreateTexture() failed!!");
			return false;
		}

		if (mHasMipmaps)
		{
			for (auto& mipmap : mipmaps)
			{
				upload(mipmap.level, mipmap.width, mipmap.height, image->getChannels(),
					mipmap.data.data(), (uint32_t)mipmap.data.size());
			}
		}
		else
		{
			const auto& imageData = image->getData();
			upload(image->getChannels(), imageData.data(), (uint32_t)imageData.size());
		}

		wgpu::TextureViewDescriptor textureViewDesc = {
			.format = mFormat,
			.dimension = wgpu::TextureViewDimension::e2D,
			.baseMipLevel = 0,
			.mipLevelCount = 1,
			.baseArrayLayer = 0,
			.arrayLayerCount = 1
		};

		mView = mHandle.CreateView(&textureViewDesc);
		if (!mView)
		{
			LogError("wgpu::Texture::CreateView() failed!!");
			return false;
		}

		return true;
	}

	void Texture::upload(uint32_t channels, const void* data, uint32_t size)
	{
		upload(0, mWidth, mHeight, channels, data, size);
	}

	void Texture::upload(uint32_t level, uint32_t width, uint32_t height, uint32_t channels,
		const void* data, uint32_t size)
	{
		wgpu::ImageCopyTexture destination = {
			.texture = mHandle,
			.mipLevel = level,
			.aspect = wgpu::TextureAspect::All
		};

		wgpu::TextureDataLayout dataLayout = {
			.offset = 0,
			.bytesPerRow = width * channels,
			.rowsPerImage = height
		};

		wgpu::Extent3D texSize = {
			.width = width,
			.height = height,
			.depthOrArrayLayers = 1
		};

		const wgpu::Queue& queue = GraphicsDevice::get().getQueue();
		queue.WriteTexture(&destination, data, size, &dataLayout, &texSize);
	}

    std::type_index Texture::getType() const
    {
        return typeid(Texture);
    }
}