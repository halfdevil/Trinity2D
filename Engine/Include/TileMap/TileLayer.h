#pragma once

#include "VFS/Serializer.h"
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>

namespace Trinity
{
	class TileLayerSerializer;

	struct TileObject
	{
		glm::vec2 position;
		glm::vec2 size;
		std::unordered_map<std::string, std::string> properties;
	};

	class TileLayer
	{
	public:

		friend class TileLayerSerializer;

		TileLayer() = default;
		virtual ~TileLayer() = default;

		TileLayer(const TileLayer&) = delete;
		TileLayer& operator = (const TileLayer&) = delete;

		TileLayer(TileLayer&&) = default;
		TileLayer& operator = (TileLayer&&) = default;

		const std::string& getName() const
		{
			return mName;
		}

		const std::vector<uint32_t>& getData() const
		{
			return mData;
		}

		const std::vector<TileObject>& getObjects() const
		{
			return mObjects;
		}

		bool isVisible() const
		{
			return mVisible;
		}

		virtual ISerializer* getSerializer();
		virtual uint32_t getTile(uint32_t index);

		virtual void setName(const std::string& name);
		virtual void setData(std::vector<uint32_t>&& data);
		virtual void setObjects(std::vector<TileObject>&& objects);
		virtual void setTile(uint32_t index, uint32_t id);
		virtual void setVisible(bool visible);
		virtual void toggleVisibility();
		virtual void resize(uint32_t newSize);

	protected:

		std::string mName;
		std::vector<uint32_t> mData;
		std::vector<TileObject> mObjects;
		bool mVisible{ true };
	};

	class TileLayerSerializer : public ISerializer
	{
	public:

		TileLayerSerializer() = default;
		virtual ~TileLayerSerializer() = default;

		TileLayerSerializer(const TileLayerSerializer&) = delete;
		TileLayerSerializer& operator = (const TileLayerSerializer&) = delete;

		TileLayerSerializer(TileLayerSerializer&&) = default;
		TileLayerSerializer& operator = (TileLayerSerializer&&) = default;

		virtual void setTileLayer(TileLayer& tileLayer);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		virtual bool readTileObject(FileReader& reader, TileObject& tileObject);
		virtual bool writeTileObject(FileWriter& writer, const TileObject& tileObject);

		virtual bool readTileObject(json& object, TileObject& tileObject);
		virtual bool writeTileObject(json& object, const TileObject& tileObject);

	protected:

		TileLayer* mTileLayer{ nullptr };
	};
}