#include "TileMap/TileMap.h"
#include "TileMap/TileLayer.h"
#include "TileMap/TileSet.h"
#include "Graphics/BatchRenderer.h"
#include <algorithm>

namespace Trinity
{
	std::vector<TileSet*> TileMap::getTileSets() const
	{
		std::vector<TileSet*> result(mTileSets.size());
		std::transform(mTileSets.begin(), mTileSets.end(), result.begin(),
			[](auto& tileSet) {
				return tileSet.get();
			}
		);

		return result;
	}

	std::vector<TileLayer*> TileMap::getTileLayers() const
	{
		std::vector<TileLayer*> result(mTileLayers.size());
		std::transform(mTileLayers.begin(), mTileLayers.end(), result.begin(),
			[](auto& tileLayer) {
				return tileLayer.get();
			}
		);

		return result;
	}

	uint32_t TileMap::getTile(uint32_t layerIdx, uint32_t x, uint32_t y)
	{
		if (layerIdx < (uint32_t)mTileLayers.size())
		{
			const auto& data = mTileLayers[layerIdx]->getData();
			return data[y * mNumTiles.x + x];
		}

		return 0;
	}

	TileSet* TileMap::getTileSet(uint32_t tileId)
	{
		for (auto it = mTileSets.rbegin(); it != mTileSets.rend(); it++)
		{
			auto& tileSet = *it;
			if (tileSet->getFirstId() <= tileId)
			{
				return tileSet.get();
			}
		}

		return nullptr;
	}

	void TileMap::setTileSize(const glm::vec2& tileSize)
	{
		mTileSize = tileSize;
	}

	void TileMap::setSize(const glm::vec2& size)
	{
		mSize = size;
	}

	void TileMap::setNumTiles(const glm::uvec2& numTiles)
	{
		mNumTiles = numTiles;
	}

	void TileMap::addTileSet(std::unique_ptr<TileSet> tileSet)
	{
		mTileSets.push_back(std::move(tileSet));
	}

	void TileMap::addTileLayer(std::unique_ptr<TileLayer> tileLayer)
	{
		mTileLayers.push_back(std::move(tileLayer));
	}

	void TileMap::draw(BatchRenderer& batchRenderer, float x, float y, float width, float height)
	{
		for (auto& tileLayer : mTileLayers)
		{
			const auto& tileData = tileLayer->getData();
			for (uint32_t idx = 0; idx < (uint32_t)tileData.size(); idx++)
			{
				auto tileId = tileData[idx];
				if (tileId == 0)
				{
					continue;
				}

				auto* tileSet = getTileSet(tileId);
				auto srcPosition = tileSet->getPosition(tileId);
				const auto& srcSize = tileSet->getTileSize();

				auto dstPosition = glm::vec2{
					std::floorf(idx % mNumTiles.x) * mTileSize.x,
					std::floorf(idx / mNumTiles.y) * mTileSize.y
				};

				if ((dstPosition.x + mTileSize.x < x || dstPosition.y + mTileSize.y < y) ||
					(dstPosition.x > x + width || dstPosition.y > y + height))
				{
					continue;
				}

				batchRenderer.drawTexture(tileSet->getTexture(), srcPosition, srcSize,
					dstPosition, dstPosition);
			}
		}
	}
}