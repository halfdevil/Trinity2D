#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>

namespace Trinity
{
	struct TileObject
	{
		glm::vec2 position;
		glm::vec2 size;
		std::unordered_map<std::string, std::string> properties;
	};

	class TileLayer
	{
	public:

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

		virtual void setName(const std::string& name);
		virtual void setData(std::vector<uint32_t>&& data);
		virtual void setObjects(std::vector<TileObject>&& objects);
		virtual void setTile(uint32_t index, uint32_t id);

	protected:

		std::string mName;
		std::vector<uint32_t> mData;
		std::vector<TileObject> mObjects;
	};
}