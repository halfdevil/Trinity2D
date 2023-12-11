#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>

namespace Trinity
{
	class TileSet;
	class TileLayer;
	class BatchRenderer;

	class TileMap
	{
	public:

		TileMap() = default;
		virtual ~TileMap() = default;

		TileMap(const TileMap&) = delete;
		TileMap& operator = (const TileMap&) = delete;

		TileMap(TileMap&&) = default;
		TileMap& operator = (TileMap&&) = default;

		const glm::vec2& getTileSize() const
		{
			return mTileSize;
		}

		const glm::vec2& getSize() const
		{
			return mSize;
		}

		const glm::uvec2& getNumTiles() const
		{
			return mNumTiles;
		}

		virtual std::vector<TileSet*> getTileSets() const;
		virtual std::vector<TileLayer*> getTileLayers() const;

		virtual uint32_t getTile(uint32_t layerIdx, uint32_t x, uint32_t y);
		virtual TileSet* getTileSet(uint32_t tileId);

		virtual void setTileSize(const glm::vec2& tileSize);
		virtual void setSize(const glm::vec2& size);
		virtual void setNumTiles(const glm::uvec2& numTiles);
		virtual void addTileSet(std::unique_ptr<TileSet> tileSet);
		virtual void addTileLayer(std::unique_ptr<TileLayer> tileLayer);

		virtual void draw(BatchRenderer& batchRenderer,	float x,float y,
			float width, float height);

	protected:

		glm::vec2 mTileSize{ 0.0f };
		glm::vec2 mSize{ 0.0f };
		glm::uvec2 mNumTiles{ 0 };
		std::vector<std::unique_ptr<TileSet>> mTileSets;
		std::vector<std::unique_ptr<TileLayer>> mTileLayers;
	};
}