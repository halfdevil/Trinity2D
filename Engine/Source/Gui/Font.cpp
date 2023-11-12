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

	bool Font::create(const std::string& fileName, float size, uint32_t width, uint32_t height, 
		uint32_t numGlyphs, uint32_t firstGlyph)
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

		stbtt_PackBegin(&context, bitmap.data(), width, height, 0, 1, nullptr);
		stbtt_PackSetOversampling(&context, 1, 1);
		stbtt_PackFontRange(&context, buffer.data(), 0, size, firstGlyph, numGlyphs, chars.data());
		stbtt_PackEnd(&context);

		mSize = size;
		mNumGlyphs = numGlyphs;

		uint32_t index{ 0 };
		for (auto& charInfo : chars)
		{
			mCharInfos.push_back({
				.id = index++,
				.x0 = (float)charInfo.x0,// / (float)width,
				.y0 = (float)charInfo.y0,// / (float)height,
				.x1 = (float)charInfo.x1,// / (float)width,
				.y1 = (float)charInfo.y1,// / (float)height,
				.xoff = charInfo.xoff,// / (float)width,
				.yoff = charInfo.yoff,// / (float)height,
				.xoff2 = charInfo.xoff2,// / (float)width,
				.yoff2 = charInfo.yoff2,// / (float)height,
				.xadvance = charInfo.xadvance,// / (float)width
			});
		}

		stbtt_fontinfo info{};
		stbtt_InitFont(&info, buffer.data(), stbtt_GetFontOffsetForIndex(buffer.data(), 0));

		int ascent;
		int descent;
		int lineGap;

		stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
		float scale = stbtt_ScaleForPixelHeight(&info, size);

		mAscent = scale * ascent;
		mDescent = scale * descent;
		mLineGap = scale * lineGap;
		mLineDistance = mAscent - mDescent + mLineGap;

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
		mCharInfos.clear();
	}

	std::type_index Font::getType() const
	{
		return typeid(Font);
	}
}