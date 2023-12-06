#include "Core/Atlas.h"
#include "Core/Image.h"
#include "Core/Logger.h"
#include <algorithm>
#include "stb_rect_pack.h"

namespace Trinity
{
	bool Atlas::create(uint32_t width, uint32_t height, uint32_t channels, const std::vector<const Image*>& images)
	{
		struct ImageInfo
		{
			uint32_t index;
			const Image* image;
		};

		bool done{ false };	
		std::vector<stbrp_node> nodes(kMaxWidth);
		std::vector<stbrp_rect> rects(kMaxImages);

		std::vector<ImageInfo> imageInfos(images.size());
		for (uint32_t idx = 0; idx < (uint32_t)images.size(); idx++)
		{
			imageInfos[idx] = { idx, images[idx] };
		}

		mAtlasRects.resize(images.size());

		while (!done)
		{
			uint32_t numImages = (uint32_t)imageInfos.size();
			for (uint32_t idx = 0; idx < numImages; idx++)
			{
				const auto* image = imageInfos[idx].image;
				auto& rect = rects[idx];

				rect.id = idx;
				rect.w = image->getWidth();
				rect.h = image->getHeight();
			}

			stbrp_context ctx;
			stbrp_init_target(&ctx, width, height, nodes.data(), kMaxWidth);
			stbrp_pack_rects(&ctx, rects.data(), numImages);

			auto atlasImage = std::make_unique<Image>();
			if (!atlasImage->create(width, height, channels))
			{
				LogError("Image::create() failed for atlas");
				return false;
			}

			std::vector<ImageInfo> failedImages;
			for (uint32_t idx = 0; idx < numImages; idx++)
			{
				if (!rects[idx].was_packed)
				{
					failedImages.push_back(imageInfos[rects[idx].id]);
				}
				else
				{
					const auto& rect = rects[idx];
					const auto* image = imageInfos[rect.id].image;
					atlasImage->blit(*image, rect.x, rect.y);

					mAtlasRects[imageInfos[rect.id].index] = { 
						(uint32_t)mAtlasImages.size(), 
						(uint32_t)rect.x, 
						(uint32_t)rect.y, 
						image->getWidth(), 
						image->getHeight() 
					};
				}
			}

			if (failedImages.size() > 0)
			{
				imageInfos = std::move(failedImages);
			}
			else
			{
				done = true;
			}

			mAtlasImages.push_back(std::move(atlasImage));
		}

		return true;
	}
	
	void Atlas::destroy()
	{
		mAtlasImages.clear();
		mAtlasRects.clear();
	}

	std::vector<Image*> Atlas::getAtlasImages() const
	{
		std::vector<Image*> result(mAtlasImages.size());
		std::transform(mAtlasImages.begin(), mAtlasImages.end(), result.begin(), 
			[](const auto& image) {
				return image.get();
			}
		);
		
		return result;
	}
}