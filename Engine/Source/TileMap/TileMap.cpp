#include "TileMap/TileMap.h"
#include "TileMap/TileLayer.h"
#include "TileMap/TileSet.h"
#include "Graphics/BatchRenderer.h"
#include "Editor/EditorLayout.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/Logger.h"
#include <algorithm>

namespace Trinity
{
	std::type_index TileMap::getType() const
	{
		return typeid(TileMap);
	}

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

	IEditor* TileMap::getEditor(uint32_t selectedTileSet)
	{
		static TileMapEditor editor;
		editor.setTileMap(*this);
		editor.setSelectedTileSet(selectedTileSet);

		return &editor;
	}

	ISerializer* TileMap::getSerializer()
	{
		static TileMapSerializer serializer;
		serializer.setTileMap(*this);

		return &serializer;
	}

	uint32_t TileMap::getTotalTiles() const
	{
		return mNumTiles.x * mNumTiles.y;
	}

	uint32_t TileMap::getTile(uint32_t layerIdx, uint32_t x, uint32_t y) const
	{
		return getTile(layerIdx, y * mNumTiles.x + x);
	}

	uint32_t TileMap::getTile(uint32_t layerIdx, uint32_t tileIdx) const
	{
		if (layerIdx < (uint32_t)mTileLayers.size())
		{
			return mTileLayers.at(layerIdx)->getTile(tileIdx);
		}

		return 0;
	}

	TileSet* TileMap::getTileSet(uint32_t index) const
	{
		if (index < (uint32_t)mTileSets.size())
		{
			return mTileSets.at(index).get();
		}

		return nullptr;
	}

	TileSet* TileMap::getTileSetFromId(uint32_t tileId) const
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

	uint32_t TileMap::getNextTileSetId() const
	{
		uint32_t nextId{ 0 };

		for (auto& tileSet : mTileSets)
		{
			const auto& numTiles = tileSet->getNumTiles();
			auto firstId = tileSet->getFirstId();
			nextId += (firstId + (numTiles.x * numTiles.y));
		}

		return nextId;
	}

	void TileMap::setTile(uint32_t layerIdx, uint32_t x, uint32_t y, uint32_t tileId)
	{
		setTile(layerIdx, y * mNumTiles.x + x, tileId);
	}

	void TileMap::setTile(uint32_t layerIdx, uint32_t tileIdx, uint32_t tileId)
	{
		if (layerIdx < (uint32_t)mTileLayers.size())
		{
			mTileLayers.at(layerIdx)->setTile(tileIdx, tileId);
		}
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
		if (mNumTiles.x > 0 && mNumTiles.y > 0)
		{
			tileLayer->resize(mNumTiles.x * mNumTiles.y);
		}

		mTileLayers.push_back(std::move(tileLayer));
	}

	void TileMap::removeTileSet(uint32_t tileSetIndex)
	{
		if (tileSetIndex < (uint32_t)mTileSets.size())
		{
			mTileSets.erase(mTileSets.begin() + tileSetIndex);
		}
	}

	void TileMap::removeTileLayer(uint32_t tileLayerIndex)
	{
		if (tileLayerIndex < (uint32_t)mTileLayers.size())
		{
			mTileLayers.erase(mTileLayers.begin() + tileLayerIndex);
		}
	}

	void TileMap::moveTileSet(uint32_t from, uint32_t to)
	{
		if (from < (uint32_t)mTileSets.size() && to < (uint32_t)mTileSets.size())
		{
			std::swap(mTileSets[from], mTileSets[to]);
		}
	}

	void TileMap::moveTileLayer(uint32_t from, uint32_t to)
	{
		if (from < (uint32_t)mTileLayers.size() && to < (uint32_t)mTileLayers.size())
		{
			std::swap(mTileLayers[from], mTileLayers[to]);
		}
	}

	void TileMap::draw(BatchRenderer& batchRenderer, const glm::vec2& position,
		float left, float bottom, float right, float top)
	{
		for (auto& tileLayer : mTileLayers)
		{
			if (!tileLayer->isVisible())
			{
				continue;
			}

			const auto& tileData = tileLayer->getData();
			for (uint32_t idx = 0; idx < (uint32_t)tileData.size(); idx++)
			{
				auto tileId = tileData[idx];
				if (tileId == 0)
				{
					continue;
				}

				auto* tileSet = getTileSetFromId(tileId);
				if (tileSet != nullptr)
				{
					auto srcPosition = tileSet->getPosition(tileId);
					const auto& srcSize = tileSet->getTileSize();

					auto dstPosition = position + glm::vec2{
						(float)(idx % mNumTiles.x) * mTileSize.x,
						(float)(idx / mNumTiles.x) * mTileSize.y
					};

					if ((dstPosition.x + mTileSize.x < left || dstPosition.y + mTileSize.y < bottom) ||
						(dstPosition.x > right || dstPosition.y > top))
					{
						continue;
					}

					batchRenderer.drawTexture(tileSet->getTexture(), srcPosition, srcSize,
						dstPosition, mTileSize);
				}
			}
		}
	}

	void TileMapSerializer::setTileMap(TileMap& tileMap)
	{
		mTileMap = &tileMap;
	}

	bool TileMapSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		if (!reader.read(&mTileMap->mTileSize))
		{
			LogError("FileReader::read() failed for 'tile size'");
			return false;
		}

		if (!reader.read(&mTileMap->mNumTiles))
		{
			LogError("FileReader::read() failed for 'num tiles'");
			return false;
		}

		uint32_t numTileSets{ 0 };
		if (!reader.read(&numTileSets))
		{
			LogError("FileReader::read() failed for 'num tile sets'");
			return false;
		}

		for (uint32_t idx = 0; idx < numTileSets; idx++)
		{
			auto tileSet = std::make_unique<TileSet>();
			if (auto* serializer = tileSet->getSerializer(); serializer != nullptr)
			{
				if (!serializer->read(reader, cache))
				{
					LogError("TileSetSerializer::read() failed");
					return false;
				}
			}

			mTileMap->mTileSets.push_back(std::move(tileSet));
		}

		uint32_t numTileLayers{ 0 };
		if (!reader.read(&numTileLayers))
		{
			LogError("FileReader::read() failed for 'num tile layers'");
			return false;
		}

		for (uint32_t idx = 0; idx < numTileLayers; idx++)
		{
			auto tileLayer = std::make_unique<TileLayer>();
			if (auto* serializer = tileLayer->getSerializer(); serializer != nullptr)
			{
				if (!serializer->read(reader, cache))
				{
					LogError("TileLayerSerializer::read() failed");
					return false;
				}
			}

			mTileMap->mTileLayers.push_back(std::move(tileLayer));
		}

		mTileMap->mSize = {
			mTileMap->mNumTiles.x * mTileMap->mTileSize.x,
			mTileMap->mNumTiles.y * mTileMap->mTileSize.y
		};

		return true;
	}

	bool TileMapSerializer::write(FileWriter& writer)
	{
		if (!writer.write(&mTileMap->mTileSize))
		{
			LogError("FileWriter::write() failed for 'tile size'");
			return false;
		}

		if (!writer.write(&mTileMap->mNumTiles))
		{
			LogError("FileWriter::write() failed for 'num tiles'");
			return false;
		}

		const auto numTileSets = (uint32_t)mTileMap->mTileSets.size();
		if (!writer.write(&numTileSets))
		{
			LogError("FileWriter::write() failed for 'num tilesets'");
			return false;
		}

		for (auto& tileSet : mTileMap->mTileSets)
		{
			if (auto* serializer = tileSet->getSerializer(); serializer != nullptr)
			{
				if (!serializer->write(writer))
				{
					LogError("TileSetSerializer::write() failed");
					return false;
				}
			}
		}

		const auto numTileLayers = (uint32_t)mTileMap->mTileLayers.size();
		if (!writer.write(&numTileLayers))
		{
			LogError("FileWriter::write() failed for 'num tilelayers'");
			return false;
		}

		for (auto& tileLayer : mTileMap->mTileLayers)
		{
			if (auto* serializer = tileLayer->getSerializer(); serializer != nullptr)
			{
				if (!serializer->write(writer))
				{
					LogError("TileLayerSerializer::write() failed");
					return false;
				}
			}
		}

		return true;
	}

	bool TileMapSerializer::read(json& object, ResourceCache& cache)
	{
		if (!object.contains("tileSize"))
		{
			LogError("JSON TileSet object doesn't have 'tile size' key");
			return false;
		}

		if (!object.contains("numTiles"))
		{
			LogError("JSON TileSet object doesn't have 'tile size' key");
			return false;
		}

		if (!object.contains("tileSets"))
		{
			LogError("JSON TileSet object doesn't have 'tile size' key");
			return false;
		}

		if (!object.contains("tileLayers"))
		{
			LogError("JSON TileSet object doesn't have 'tile size' key");
			return false;
		}

		mTileMap->mTileSize = {
			object["tileSize"][0].get<float>(),
			object["tileSize"][1].get<float>()
		};

		mTileMap->mNumTiles = {
			object["numTiles"][0].get<uint32_t>(),
			object["numTiles"][1].get<uint32_t>()
		};

		for (auto& tileSetJson : object["tileSets"])
		{
			auto tileSet = std::make_unique<TileSet>();
			if (auto* serializer = tileSet->getSerializer(); serializer != nullptr)
			{
				if (!serializer->read(tileSetJson, cache))
				{
					LogError("TileSetSerializer::read() failed");
					return false;
				}
			}

			mTileMap->mTileSets.push_back(std::move(tileSet));
		}

		for (auto& tileLayerJson : object["tileLayers"])
		{
			auto tileLayer = std::make_unique<TileLayer>();
			if (auto* serializer = tileLayer->getSerializer(); serializer != nullptr)
			{
				if (!serializer->read(tileLayerJson, cache))
				{
					LogError("TileLayerSerializer::read() failed");
					return false;
				}
			}

			mTileMap->mTileLayers.push_back(std::move(tileLayer));
		}

		mTileMap->mSize = {
			mTileMap->mNumTiles.x * mTileMap->mTileSize.x,
			mTileMap->mNumTiles.y * mTileMap->mTileSize.y
		};

		return true;
	}

	bool TileMapSerializer::write(json& object)
	{
		object["tileSize"] = std::vector<float>{
			mTileMap->mTileSize.x,
			mTileMap->mTileSize.y
		};

		object["numTiles"] = std::vector<uint32_t>{
			mTileMap->mNumTiles.x,
			mTileMap->mNumTiles.y
		};

		json tileSetsJson;
		for (auto& tileSet : mTileMap->mTileSets)
		{
			json tileSetJson;
			if (auto* serializer = tileSet->getSerializer(); serializer != nullptr)
			{
				if (!serializer->write(tileSetJson))
				{
					LogError("TileSetSerializer::write() failed");
					return false;
				}
			}

			tileSetsJson.push_back(std::move(tileSetJson));
		}

		json tileLayersJson;
		for (auto& tileLayer : mTileMap->mTileLayers)
		{
			json tileLayerJson;
			if (auto* serializer = tileLayer->getSerializer(); serializer != nullptr)
			{
				if (!serializer->write(tileLayerJson))
				{
					LogError("TileLayerSerializer::write() failed");
					return false;
				}
			}

			tileLayersJson.push_back(std::move(tileLayerJson));
		}

		object["tileSets"] = std::move(tileSetsJson);
		object["tileLayers"] = std::move(tileLayersJson);

		return true;
	}

	void TileMapEditor::setTileMap(TileMap& tileMap)
	{
		mTileMap = &tileMap;
	}

	void TileMapEditor::setSelectedTileSet(uint32_t selectedTileSet)
	{
		if (selectedTileSet < (uint32_t)mTileMap->mTileSets.size())
		{
			mSelectedTileSetIndex = selectedTileSet;
			mSelectedTileSet = mTileMap->mTileSets.at(selectedTileSet).get();
		}
		else
		{
			mSelectedTileSet = nullptr;
		}
	}

	void TileMapEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{
		if (layout.beginLayout("Tile Map"))
		{
			if (layout.inputVec2("Tile Size", mTileMap->mTileSize))
			{
				updateTileMapSize();
			}

			if (layout.inputSize("No. of Tiles", mTileMap->mNumTiles))
			{
				updateTileMapSize();
			}

			layout.endLayout();
		}

		if (mSelectedTileSet != nullptr)
		{
			if (auto* editor = mSelectedTileSet->getEditor(); editor != nullptr)
			{
				editor->onInspectorGui(layout, cache);
			}
		}
	}

	void TileMapEditor::updateTileMapSize()
	{
		mTileMap->mSize = {
			mTileMap->mTileSize.x * mTileMap->mNumTiles.x,
			mTileMap->mTileSize.y * mTileMap->mNumTiles.y
		};
	}
}