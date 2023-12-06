#pragma once

#include "Core/Resource.h"
#include <cstdint>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Trinity
{
	struct Mipmap
	{
		uint32_t width{ 0 };
		uint32_t height{ 0 };
		uint32_t level{ 0 };
		std::vector<uint8_t> data;
	};

	class Image : public Resource
	{
	public:

		Image() = default;
		virtual ~Image() = default;

		Image(const Image&) = delete;
		Image& operator = (const Image&) = delete;

		Image(Image&&) = default;
		Image& operator = (Image&&) = default;

		uint32_t getWidth() const
		{
			return mWidth;
		}

		uint32_t getHeight() const
		{
			return mHeight;
		}

		uint32_t getChannels() const
		{
			return mChannels;
		}

		const std::vector<uint8_t>& getData() const
		{
			return mData;
		}

		virtual bool create(const std::string& filePath);
		virtual bool create(std::vector<uint8_t>&& data);
		virtual bool create(uint32_t width, uint32_t height, uint32_t channels, const uint8_t* data = nullptr);
		
		virtual void destroy();
		virtual void blit(const Image& image, uint32_t x, uint32_t y);

		virtual std::type_index getType() const override;
		virtual std::vector<Mipmap> generateMipmaps() const;

		virtual glm::vec4 getPixel(uint32_t x, uint32_t y) const;
		virtual uint32_t getPixelAsRGBA(uint32_t x, uint32_t y) const;
		virtual void setPixel(uint32_t x, uint32_t y, const glm::vec4& c);

	protected:

		uint32_t mWidth{ 0 };
		uint32_t mHeight{ 0 };
		uint32_t mChannels{ 0 };
		std::vector<uint8_t> mData;
	};
}