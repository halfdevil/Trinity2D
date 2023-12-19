#pragma once

#include "Core/Resource.h"
#include "Editor/Editor.h"
#include "VFS/Serializer.h"
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
	class TileMapEditor;
	class TileMapSerializer;

	class TileMap : public Resource
	{
	public:
		
		friend class TileMapEditor;
		friend class TileMapSerializer;

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

		uint32_t getNumTileSet() const
		{
			return (uint32_t)mTileSets.size();
		}

		uint32_t getNumTileLayers() const
		{
			return (uint32_t)mTileLayers.size();
		}

		virtual std::type_index getType() const override;

		virtual std::vector<TileSet*> getTileSets() const;
		virtual std::vector<TileLayer*> getTileLayers() const;
		virtual IEditor* getEditor(uint32_t selectedTileSet);
		virtual ISerializer* getSerializer();

		virtual uint32_t getTotalTiles() const;
		virtual uint32_t getTile(uint32_t layerIdx, uint32_t tileIdx) const;
		virtual uint32_t getTile(uint32_t layerIdx, uint32_t x, uint32_t y) const;
		virtual TileSet* getTileSet(uint32_t index) const;
		virtual TileSet* getTileSetFromId(uint32_t tileId) const;
		virtual uint32_t getNextTileSetId() const;

		virtual void setTile(uint32_t layerIdx, uint32_t tileIdx, uint32_t tileId);
		virtual void setTile(uint32_t layerIdx, uint32_t x, uint32_t y, uint32_t tileId);
		virtual void setTileSize(const glm::vec2& tileSize);
		virtual void setSize(const glm::vec2& size);
		virtual void setNumTiles(const glm::uvec2& numTiles);
		virtual void addTileSet(std::unique_ptr<TileSet> tileSet);
		virtual void addTileLayer(std::unique_ptr<TileLayer> tileLayer);

		virtual void removeTileSet(uint32_t tileSetIndex);
		virtual void removeTileLayer(uint32_t tileLayerIndex);
		virtual void moveTileSet(uint32_t from, uint32_t to);
		virtual void moveTileLayer(uint32_t from, uint32_t to);

		virtual void draw(BatchRenderer& batchRenderer, const glm::vec2& position, 
			float left, float bottom, float right, float top);

	protected:

		glm::vec2 mTileSize{ 0.0f };
		glm::vec2 mSize{ 0.0f };
		glm::uvec2 mNumTiles{ 0 };
		std::vector<std::unique_ptr<TileSet>> mTileSets;
		std::vector<std::unique_ptr<TileLayer>> mTileLayers;
	};

	class TileMapEditor : public IEditor
	{
	public:

		TileMapEditor() = default;
		virtual ~TileMapEditor() = default;

		TileMapEditor(const TileMapEditor&) = delete;
		TileMapEditor& operator = (const TileMapEditor&) = delete;

		TileMapEditor(TileMapEditor&&) = default;
		TileMapEditor& operator = (TileMapEditor&&) = default;

		virtual void setTileMap(TileMap& tileMap);
		virtual void setSelectedTileSet(uint32_t selectedTileSet);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		virtual void updateTileMapSize();

	protected:

		TileMap* mTileMap{ nullptr };
		TileSet* mSelectedTileSet{ nullptr };
		uint32_t mSelectedTileSetIndex{ 0 };
		std::string mSelectedTextureFile;
	};

	class TileMapSerializer : public ISerializer
	{
	public:

		TileMapSerializer() = default;
		virtual ~TileMapSerializer() = default;

		TileMapSerializer(const TileMapSerializer&) = delete;
		TileMapSerializer& operator = (const TileMapSerializer&) = delete;

		TileMapSerializer(TileMapSerializer&&) = default;
		TileMapSerializer& operator = (TileMapSerializer&&) = default;

		virtual void setTileMap(TileMap& tileMap);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		TileMap* mTileMap{ nullptr };
	};
}