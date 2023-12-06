#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace Trinity
{
	class Image;

	struct AtlasRect
	{
		uint32_t index{ 0 };
		uint32_t x{ 0 };
		uint32_t y{ 0 };
		uint32_t width{ 0 };
		uint32_t height{ 0 };
	};

	class Atlas
	{
	public:

		static constexpr uint32_t kMaxWidth = 4096;
		static constexpr uint32_t kMaxHeight = 4096;
		static constexpr uint32_t kMaxImages = 2000;

		Atlas() = default;
		virtual ~Atlas() = default;

		Atlas(const Atlas&) = delete;
		Atlas& operator = (const Atlas&) = delete;

		Atlas(Atlas&&) = default;
		Atlas& operator = (Atlas&&) = default;

		const std::vector<AtlasRect>& getAtlasRects() const
		{
			return mAtlasRects;
		}

		virtual bool create(uint32_t width, uint32_t height, uint32_t channels, const std::vector<const Image*>& images);
		virtual void destroy();

		std::vector<Image*> getAtlasImages() const;

	protected:

		std::vector<std::unique_ptr<Image>> mAtlasImages;
		std::vector<AtlasRect> mAtlasRects;
	};
}