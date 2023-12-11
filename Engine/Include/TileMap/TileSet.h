#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>

namespace Trinity
{
	class Texture;

	struct TileProperty
	{
		std::string name;
		std::string value;
	};

	class TileSet
	{
	public:

		TileSet() = default;
		virtual ~TileSet() = default;

		TileSet(const TileSet&) = delete;
		TileSet& operator = (const TileSet&) = delete;

		TileSet(TileSet&&) = default;
		TileSet& operator = (TileSet&&) = default;

		const std::string& getName() const
		{
			return mName;
		}

		const glm::vec2& getSize() const
		{
			return mSize;
		}

		const glm::vec2& getTileSize() const
		{
			return mTileSize;
		}

		const glm::uvec2& getNumTiles() const
		{
			return mNumTiles;
		}

		float getSpacing() const
		{
			return mSpacing;
		}

		uint32_t getFirstId() const
		{
			return mFirstId;
		}

		Texture* getTexture() const
		{
			return mTexture;
		}

		const std::vector<TileProperty>& getProperties(uint32_t id) const
		{
			return mProperties.at(id);
		}

		virtual glm::vec2 getPosition(uint32_t globalId) const;

		virtual void setName(const std::string& name);
		virtual void setSize(const glm::vec2& size);
		virtual void setTileSize(const glm::vec2& tileSize);
		virtual void setNumTiles(const glm::uvec2& numTiles);
		virtual void setSpacing(float spacing);
		virtual void setFirstId(uint32_t id);
		virtual void setTexture(Texture* texture);

		virtual void addProperty(uint32_t id, const TileProperty& property);
		virtual void setProperties(uint32_t id, std::vector<TileProperty>&& properties);

	protected:

		std::string mName;
		glm::vec2 mSize{ 0.0f };
		glm::vec2 mTileSize{ 0.0f };
		glm::uvec2 mNumTiles{ 0 };
		float mSpacing{ 0.0f };
		uint32_t mFirstId{ 0 };
		Texture* mTexture{ nullptr };
		std::unordered_map<uint32_t, std::vector<TileProperty>> mProperties;
	};
}