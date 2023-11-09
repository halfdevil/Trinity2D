#pragma once 

#include "Core/Resource.h"
#include <vector>
#include <memory>

namespace Trinity
{
	class Texture;

	class Font : public Resource
	{
	public:

		struct CharInfo
		{
			uint32_t id{ 0 };
			float x0{ 0 };
			float y0{ 0 };
			float x1{ 0 };
			float y1{ 0 };
			float xoff{ 0.0f };
			float yoff{ 0.0f };
			float xoff2{ 0.0f };
			float yoff2{ 0.0f };
			float xadvance{ 0.0f };
		};

		Font() = default;
		~Font();

		Font(const Font&) = delete;
		Font& operator = (const Font&) = delete;

		Font(Font&&) = default;
		Font& operator = (Font&&) = default;

		uint32_t getNumGlyphs() const
		{
			return mNumGlyphs;
		}

		float getSize() const
		{
			return mSize;
		}

		float getAscent() const
		{
			return mAscent;
		}

		float getDescent() const
		{
			return mDescent;
		}

		float getLineGap() const
		{
			return mLineGap;
		}

		float getLineDistance() const
		{
			return mLineDistance;
		}

		const std::vector<CharInfo>& getCharInfos() const
		{
			return mCharInfos;
		}

		Texture* getTexture() const
		{
			return mTexture.get();
		}

		virtual bool create(const std::string& fileName, float size, uint32_t width = 512, uint32_t height = 512, 
			uint32_t numGlyphs = 96, uint32_t firstGlyph = 32);

		virtual void destroy();
		virtual std::type_index getType() const override;

	protected:

		uint32_t mNumGlyphs{ 0 };
		float mSize{ 0.0f };
		float mAscent{ 0.0f };
		float mDescent{ 0.0f };
		float mLineGap{ 0.0f };
		float mLineDistance{ 0.0f };
		std::vector<CharInfo> mCharInfos;
		std::unique_ptr<Texture> mTexture{ nullptr };
	};
}