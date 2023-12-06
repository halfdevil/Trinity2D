#include "Core/Image.h"
#include "Core/Debugger.h"
#include "Core/Logger.h"
#include "VFS/FileSystem.h"

#define _USE_MATH_DEFINES
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_resize2.h>

namespace Trinity
{
	bool Image::create(const std::string& filePath)
	{
		auto file = FileSystem::get().openFile(filePath, FileOpenMode::OpenRead);
		if (!file)
		{
			LogError("FileSystem::openFile() failed for: %s", filePath.c_str());
			return false;
		}

		FileReader reader(*file);
		std::vector<uint8_t> buffer(reader.getSize());
		
		if (!reader.read(buffer.data(), reader.getSize()))
		{
			LogError("FileReader::read() failed for: %s", filePath.c_str());
			return false;
		}

		mFileName = filePath;
		return create(std::move(buffer));
	}

	bool Image::create(std::vector<uint8_t>&& data)
	{
		int32_t width{ 0 };
		int32_t height{ 0 };
		int32_t numChannels{ 0 };

		auto* image = stbi_load_from_memory(data.data(), (int)data.size(), &width,
			&height, &numChannels, STBI_rgb_alpha);

		if (!image)
		{
			LogError("stbi_load_from_memory() failed!!");
			return false;
		}

		mWidth = (uint32_t)width;
		mHeight = (uint32_t)height;
		mChannels = 4;

		mData.resize(mWidth * mHeight * mChannels);
		memcpy(mData.data(), image, mData.size());

		stbi_image_free(image);

		return true;
	}

	bool Image::create(uint32_t width, uint32_t height, uint32_t channels, const uint8_t* data)
	{
		mWidth = width;
		mHeight = height;
		mChannels = channels;

		const uint32_t dataSize = mWidth * mHeight * mChannels;
		mData.resize(dataSize);

		if (data)
		{
			memcpy(mData.data(), data, dataSize);
		}

		return true;
	}

	void Image::destroy()
	{
		mData.clear();
	}

	void Image::blit(const Image& image, uint32_t x, uint32_t y)
	{
		for (int32_t idx = 0; idx < (int32_t)image.mHeight; idx++)
		{
			auto dy = (int32_t)y + idx;
			if (dy < 0 || dy >= (int32_t)mHeight)
			{
				continue;
			}

			for (int32_t jdx = 0; jdx < (int32_t)image.mWidth; jdx++)
			{
				auto dx = (int32_t)x + jdx;
				if (dx < 0 || dx >= (int32_t)mWidth)
				{
					continue;;
				}

				mData[dx + dy * mWidth] = image.mData[jdx + idx * image.mWidth];
			}
		}
	}

	std::type_index Image::getType() const
	{
		return typeid(Image);
	}

	std::vector<Mipmap> Image::generateMipmaps() const
	{
		std::vector<Mipmap> mipmaps;
		mipmaps.push_back({
			.width = mWidth,
			.height = mHeight,
			.level = 0,
			.data = mData
		});

		uint32_t offset{ 0 };
		uint32_t nextWidth = std::max(1u, mWidth / 2);
		uint32_t nextHeight = std::max(1u, mHeight / 2);
		uint32_t nextSize = nextWidth * nextHeight * mChannels;

		while (true)
		{
			auto& prevMipmap = mipmaps.back();
			Mipmap nextMipmap = {
				.width = nextWidth,
				.height = nextHeight,
				.level = prevMipmap.level + 1,
				.data = std::vector<uint8_t>(nextSize)
			};

			auto& prevData = prevMipmap.data;
			auto& nextData = nextMipmap.data;

			stbir_resize_uint8_linear(prevData.data(), prevMipmap.width, prevMipmap.height, 0, nextData.data(), 
				nextMipmap.width, nextMipmap.height, 0, (stbir_pixel_layout)mChannels);

			mipmaps.push_back(std::move(nextMipmap));
			nextWidth = std::max(1u, nextWidth / 2);
			nextHeight = std::max(1u, nextWidth / 2);
			nextSize = nextWidth * nextHeight * mChannels;

			if (nextWidth == 1 && nextHeight == 1)
			{
				break;
			}
		}

		return mipmaps;
	}

	glm::vec4 Image::getPixel(uint32_t x, uint32_t y) const
	{
		const uint32_t ofs = mChannels * (y * mWidth + x);

		return {
			mChannels > 0 ? float(mData[ofs + 0]) / 255.0f : 0.0f,
			mChannels > 1 ? float(mData[ofs + 1]) / 255.0f : 0.0f,
			mChannels > 2 ? float(mData[ofs + 2]) / 255.0f : 0.0f,
			mChannels > 3 ? float(mData[ofs + 3]) / 255.0f : 0.0f
		};

		return glm::vec4(0.0f);
	}

	uint32_t Image::getPixelAsRGBA(uint32_t x, uint32_t y) const
	{
		const uint32_t ofs = mChannels * (y * mWidth + x);
		uint32_t result = 0;

		result |= (mChannels > 0) ? mData[ofs + 0] << 0 : 0;
		result |= (mChannels > 1) ? mData[ofs + 1] << 8 : 0;
		result |= (mChannels > 2) ? mData[ofs + 2] << 16 : 0;
		result |= (mChannels > 3) ? mData[ofs + 3] << 24 : 0;

		return result;
	}

	void Image::setPixel(uint32_t x, uint32_t y, const glm::vec4& c)
	{
		const uint32_t ofs = mChannels * (y * mWidth + x);

		if (mChannels > 0) mData[ofs + 0] = uint8_t(c.x * 255.0f);
		if (mChannels > 1) mData[ofs + 1] = uint8_t(c.y * 255.0f);
		if (mChannels > 2) mData[ofs + 2] = uint8_t(c.z * 255.0f);
		if (mChannels > 3) mData[ofs + 3] = uint8_t(c.w * 255.0f);
	}
}