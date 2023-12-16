#include "TileMap/TileLayer.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/Logger.h"

namespace Trinity
{
	ISerializer* TileLayer::getSerializer()
	{
		static TileLayerSerializer serializer;
		serializer.setTileLayer(*this);

		return &serializer;
	}

	uint32_t TileLayer::getTile(uint32_t index)
	{
		if (index < (uint32_t)mData.size())
		{
			return mData.at(index);
		}

		return 0;
	}

	void TileLayer::setName(const std::string& name)
	{
		mName = name;
	}

	void TileLayer::setData(std::vector<uint32_t>&& data)
	{
		mData = std::move(data);
	}

	void TileLayer::setObjects(std::vector<TileObject>&& objects)
	{
		mObjects = std::move(objects);
	}

	void TileLayer::setTile(uint32_t index, uint32_t id)
	{
		if (index < (uint32_t)mData.size())
		{
			mData[index] = id;
		}
	}

	void TileLayer::setVisible(bool visible)
	{
		mVisible = visible;
	}

	void TileLayer::toggleVisibility()
	{
		mVisible = !mVisible;
	}

	void TileLayer::resize(uint32_t newSize)
	{
		mData.resize(newSize, 0);
	}

	void TileLayerSerializer::setTileLayer(TileLayer& tileLayer)
	{
		mTileLayer = &tileLayer;
	}

	bool TileLayerSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		mTileLayer->mName = reader.readString();

		if (!reader.readVector(mTileLayer->mData))
		{
			LogError("FileReader::readVector() failed for 'data'");
			return false;
		}

		uint32_t numObjects{ 0 };
		if (!reader.read(&numObjects))
		{
			LogError("FileReader::read() failed for 'num objects'");
			return false;
		}

		if (numObjects > 0)
		{
			for (uint32_t idx = 0; idx < numObjects; idx++)
			{
				TileObject tileObject;
				if (!readTileObject(reader, tileObject))
				{
					LogError("TileLayerSerializer::readTileObject() failed");
					return false;
				}

				mTileLayer->mObjects.push_back(std::move(tileObject));
			}
		}

		return true;
	}

	bool TileLayerSerializer::write(FileWriter& writer)
	{
		if (!writer.writeString(mTileLayer->mName))
		{
			LogError("FileWriter::write() failed for 'name'");
			return false;
		}

		if (!writer.writeVector(mTileLayer->mData))
		{
			LogError("FileWriter::write() failed for 'data'");
			return false;
		}

		const auto numObjects = (uint32_t)mTileLayer->mObjects.size();
		if (!writer.write(&numObjects))
		{
			LogError("FileWriter::write() failed for 'num objects'");
			return false;
		}

		for (const auto& tileObject : mTileLayer->mObjects)
		{
			if (!writeTileObject(writer, tileObject))
			{
				LogError("TileLayerSerializer::writeTileObject() failed");
				return false;
			}
		}

		return true;
	}

	bool TileLayerSerializer::read(json& object, ResourceCache& cache)
	{
		if (!object.contains("name"))
		{
			LogError("JSON TileLayer object doesn't have 'name' key");
			return false;
		}

		if (!object.contains("data"))
		{
			LogError("JSON TileLayer object doesn't have 'data' key");
			return false;
		}

		if (!object.contains("objects"))
		{
			LogError("JSON TileLayer object doesn't have 'objects' key");
			return false;
		}

		mTileLayer->mName = object["name"].get<std::string>();
		mTileLayer->mData = object["data"].get<std::vector<uint32_t>>();

		for (auto& objectJson : object["objects"])
		{
			TileObject tileObject;
			if (!readTileObject(objectJson, tileObject))
			{
				LogError("TileLayerSerializer::readTileObject() failed");
				return false;
			}

			mTileLayer->mObjects.push_back(std::move(tileObject));
		}

		return true;
	}

	bool TileLayerSerializer::write(json& object)
	{
		json tileObjectsJson;
		for (auto& tileObject : mTileLayer->mObjects)
		{
			json tileObjectJson;
			if (!writeTileObject(tileObjectJson, tileObject))
			{
				LogError("TileLayerSerializer::writeTileObject() failed");
				return false;
			}

			tileObjectsJson.push_back(std::move(tileObjectJson));
		}

		object["name"] = mTileLayer->mName;
		object["data"] = mTileLayer->mData;
		object["objects"] = std::move(tileObjectsJson);

		return true;
	}

	bool TileLayerSerializer::readTileObject(FileReader& reader, TileObject& tileObject)
	{
		if (!reader.read(&tileObject.position))
		{
			LogError("FileReader::read() failed for 'tileObject position'");
			return false;
		}

		if (!reader.read(&tileObject.size))
		{
			LogError("FileReader::read() failed for 'tileObject size'");
			return false;
		}

		uint32_t numProperties{ 0 };
		if (!reader.read(&numProperties))
		{
			LogError("FileReader::read() failed for 'tileObject num properties'");
			return false;
		}

		if (numProperties > 0)
		{
			std::vector<std::string> keys(numProperties);
			std::vector<std::string> values(numProperties);

			for (uint32_t idx = 0; idx < numProperties; idx++)
			{
				keys[idx] = reader.readString();
			}

			for (uint32_t idx = 0; idx < numProperties; idx++)
			{
				values[idx] = reader.readString();
			}

			for (uint32_t idx = 0; idx < numProperties; idx++)
			{
				tileObject.properties.insert(std::make_pair(keys[idx], values[idx]));
			}
		}

		return true;
	}

	bool TileLayerSerializer::writeTileObject(FileWriter& writer, const TileObject& tileObject)
	{
		if (!writer.write(&tileObject.position))
		{
			LogError("FileWriter::write() failed for 'tileObject position'");
			return false;
		}

		if (!writer.write(&tileObject.size))
		{
			LogError("FileWriter::write() failed for 'tileObject size'");
			return false;
		}

		const auto numProperties = (uint32_t)tileObject.properties.size();
		if (!writer.write(&numProperties))
		{
			LogError("FileWriter::write() failed for 'tileObject num properties'");
			return false;
		}

		if (numProperties > 0)
		{
			std::vector<std::string> keys;
			std::vector<std::string> values;

			keys.reserve(numProperties);
			values.reserve(numProperties);

			for (auto& pair : tileObject.properties)
			{
				keys.push_back(pair.first);
				values.push_back(pair.second);
			}

			for (auto& key : keys)
			{
				if (!writer.writeString(key))
				{
					LogError("FileWriter::write() failed for 'tileObject property key - %s'", key.c_str());
					return false;
				}
			}

			for (auto& value : values)
			{
				if (!writer.writeString(value))
				{
					LogError("FileWriter::write() failed for 'tileObject property value - %s'", value.c_str());
					return false;
				}
			}
		}

		return true;
	}

	bool TileLayerSerializer::readTileObject(json& object, TileObject& tileObject)
	{
		if (!object.contains("position"))
		{
			LogError("JSON TileObject object doesn't have 'position' key");
			return false;
		}

		if (!object.contains("size"))
		{
			LogError("JSON TileObject object doesn't have 'size' key");
			return false;
		}

		if (!object.contains("properties"))
		{
			LogError("JSON TileObject object doesn't have 'properties' key");
			return false;
		}

		tileObject.position = {
			object["position"][0].get<float>(),
			object["position"][1].get<float>()
		};

		tileObject.size = {
			object["size"][0].get<float>(),
			object["size"][1].get<float>()
		};

		for (auto& [key, value] : object["properties"].items())
		{
			tileObject.properties.insert(std::make_pair(key, value.get<std::string>()));
		}

		return true;
	}

	bool TileLayerSerializer::writeTileObject(json& object, const TileObject& tileObject)
	{
		json propertiesJson;
		for (auto& [key, value] : tileObject.properties)
		{
			propertiesJson[key] = value;
		}

		object["position"] = std::vector<float>{
			tileObject.position.x,
			tileObject.position.y
		};

		object["size"] = std::vector<float>{
			tileObject.size.x,
			tileObject.size.y
		};

		object["properties"] = std::move(propertiesJson);

		return true;
	}
}