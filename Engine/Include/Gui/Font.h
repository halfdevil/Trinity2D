#pragma once 

#include "Core/Resource.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace Trinity
{
	class Texture;

	struct FontGlyph
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

	struct FontMetrics
	{
		float size{ 0.0f };
		float ascent{ 0.0f };
		float descent{ 0.0f };
		float lineGap{ 0.0f };
		float lineDistance{ 0.0f };
		std::vector<FontGlyph> glyphs;
	};

	class Font : public Resource
	{
	public:

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

		Texture* getTexture() const
		{
			return mTexture.get();
		}

		FontMetrics* getMetrics(float fontSize)
		{
			return &mFontMetrices.at(fontSize);
		}

		bool hasSize(float fontSize) const
		{
			return mFontMetrices.contains(fontSize);
		}

		virtual bool create(
			const std::string& fileName, 
			std::vector<float> sizes, 
			uint32_t width = 512, 
			uint32_t height = 512, 
			uint32_t numGlyphs = 96, 
			uint32_t firstGlyph = 32
		);

		virtual void destroy();
		virtual std::type_index getType() const override;

	protected:

		uint32_t mNumGlyphs{ 0 };
		std::unique_ptr<Texture> mTexture{ nullptr };
		std::unordered_map<float, FontMetrics> mFontMetrices;
	};
}