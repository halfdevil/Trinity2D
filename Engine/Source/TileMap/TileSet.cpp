#include "TileMap/TileSet.h"
#include "Graphics/Texture.h"

namespace Trinity
{
	glm::vec2 TileSet::getPosition(uint32_t globalId) const
	{
		auto localId = globalId - mFirstId;
		auto tileX = mSpacing + (mSpacing * std::floorf(localId % mNumTiles.x));
		auto tileY = mSpacing + (mSpacing * std::floorf(localId / mNumTiles.x));

		return { tileX, tileY };
	}

	void TileSet::setName(const std::string& name)
	{
		mName = name;
	}

	void TileSet::setSize(const glm::vec2& size)
	{
		mSize = size;
	}

	void TileSet::setTileSize(const glm::vec2& tileSize)
	{
		mTileSize = tileSize;
	}

	void TileSet::setNumTiles(const glm::uvec2& numTiles)
	{
		mNumTiles = numTiles;
	}

	void TileSet::setSpacing(float spacing)
	{
		mSpacing = spacing;
	}

	void TileSet::setFirstId(uint32_t id)
	{
		mFirstId = id;
	}

	void TileSet::setTexture(Texture* texture)
	{
		mTexture = texture;
	}

	void TileSet::addProperty(uint32_t id, const TileProperty& property)
	{
		if (auto it = mProperties.find(id); it != mProperties.end())
		{
			it->second.push_back(property);
		}
		else
		{
			std::vector<TileProperty> properties{ property };
			mProperties.insert(std::make_pair(id, std::move(properties)));
		}
	}

	void TileSet::setProperties(uint32_t id, std::vector<TileProperty>&& properties)
	{
		if (auto it = mProperties.find(id); it != mProperties.end())
		{
			it->second = std::move(properties);
		}
		else
		{
			mProperties.insert(std::make_pair(id, std::move(properties)));
		}
	}
}