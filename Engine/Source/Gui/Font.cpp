#include "Gui/Font.h"
#include "Graphics/Texture.h"
#include "VFS/FileSystem.h"
#include "Core/Logger.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h" 

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

namespace Trinity
{
	Font::~Font()
	{
		destroy();
	}

	bool Font::create(
		const std::string& fileName, 
		std::vector<float> sizes, 
		uint32_t width, 
		uint32_t height, 
		uint32_t numGlyphs, 
		uint32_t firstGlyph
	)
	{
		auto file = FileSystem::get().openFile(fileName, FileOpenMode::OpenRead);
		if (!file)
		{
			LogError("FileSystem::openFile() failed for: %s", fileName.c_str());
			return false;
		}

		FileReader reader(*file);
		std::vector<uint8_t> buffer(reader.getSize());
		reader.read(buffer.data(), reader.getSize());

		std::vector<uint8_t> bitmap(width * height);
		std::vector<stbtt_packedchar> chars(numGlyphs);
		stbtt_pack_context context{};

		std::vector<stbtt_pack_range> ranges(sizes.size());
		std::vector<std::vector<stbtt_packedchar>> charInfos(sizes.size());

		for (uint32_t idx = 0; idx < (uint32_t)sizes.size(); idx++)
		{
			charInfos[idx].resize(numGlyphs);
			ranges[idx] = { sizes[idx], (int)firstGlyph, nullptr, (int)numGlyphs, charInfos[idx].data(), 0, 0 };
		}

		stbtt_PackBegin(&context, bitmap.data(), width, height, 0, 1, nullptr);
		stbtt_PackSetOversampling(&context, 1, 1);
		stbtt_PackFontRanges(&context, buffer.data(), 0, ranges.data(), (int)sizes.size());
		stbtt_PackEnd(&context);

		stbtt_fontinfo info{};
		stbtt_InitFont(&info, buffer.data(), stbtt_GetFontOffsetForIndex(buffer.data(), 0));

		for (uint32_t idx = 0; idx < (uint32_t)sizes.size(); idx++)
		{
			std::vector<FontGlyph> glyphs(numGlyphs);
			for (uint32_t jdx = 0; jdx < numGlyphs; jdx++)
			{
				glyphs[jdx] = {
					.id = jdx,
					.x0 = (float)charInfos[idx][jdx].x0,
					.y0 = (float)charInfos[idx][jdx].y0,
					.x1 = (float)charInfos[idx][jdx].x1,
					.y1 = (float)charInfos[idx][jdx].y1,
					.xoff = charInfos[idx][jdx].xoff,
					.yoff = charInfos[idx][jdx].yoff,
					.xoff2 = charInfos[idx][jdx].xoff2,
					.yoff2 = charInfos[idx][jdx].yoff2,
					.xadvance = charInfos[idx][jdx].xadvance
				};
			}

			int ascent;
			int descent;
			int lineGap;

			stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
			float scale = stbtt_ScaleForPixelHeight(&info, sizes[idx]);

			mFontMetrices.insert(std::make_pair(sizes[idx], FontMetrics{
				.size = sizes[idx],
				.ascent = (float)ascent * scale,
				.descent = (float)descent * scale,
				.lineGap = (float)lineGap * scale,
				.lineDistance = (float)ascent * scale - (float)descent * scale + (float)lineGap * scale,
				.glyphs = std::move(glyphs)
			}));
		}

		const wgpu::TextureUsage usage = wgpu::TextureUsage::TextureBinding |
			wgpu::TextureUsage::CopyDst;

		mTexture = std::make_unique<Texture>();
		if (!mTexture->create(width, height, wgpu::TextureFormat::R8Unorm, usage))
		{
			LogError("Texture::create() failed");
			return false;
		}

		mTexture->upload(1, bitmap.data(), (uint32_t)bitmap.size());
		return true;
	}

	void Font::destroy()
	{
		mTexture = nullptr;
		mFontMetrices.clear();
	}

	std::type_index Font::getType() const
	{
		return typeid(Font);
	}
}