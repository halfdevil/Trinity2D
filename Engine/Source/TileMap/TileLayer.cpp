#include "TileMap/TileLayer.h"

namespace Trinity
{
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
}