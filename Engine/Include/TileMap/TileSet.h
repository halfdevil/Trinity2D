#pragma once

#include "Editor/Editor.h"
#include "VFS/Serializer.h"
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>

namespace Trinity
{
	class Texture;
	class TileEditor;
	class TileSetSerializer;

	class TileSet
	{
	public:

		friend class TileSetEditor;
		friend class TileSetSerializer;

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

		bool hasProperties(uint32_t id) const
		{
			return mProperties.contains(id);
		}

		const std::unordered_map<std::string, std::string>& getProperties(uint32_t id) const
		{
			return mProperties.at(id);
		}

		virtual glm::vec2 getPosition(uint32_t globalId) const;
		virtual IEditor* getEditor();
		virtual ISerializer* getSerializer();

		virtual void setName(const std::string& name);
		virtual void setSize(const glm::vec2& size);
		virtual void setTileSize(const glm::vec2& tileSize);
		virtual void setNumTiles(const glm::uvec2& numTiles);
		virtual void setSpacing(float spacing);
		virtual void setFirstId(uint32_t id);
		virtual void setTexture(Texture* texture);

		virtual void addProperty(uint32_t id, const std::string& name, const std::string& value);
		virtual void updateProperty(uint32_t id, const std::string& name, const std::string& value);
		virtual void removeProperty(uint32_t id, const std::string& name);
		virtual void setProperties(uint32_t id, std::unordered_map<std::string, std::string>&& properties);

	protected:

		std::string mName;
		glm::vec2 mSize{ 0.0f };
		glm::vec2 mTileSize{ 0.0f };
		glm::uvec2 mNumTiles{ 0 };
		float mSpacing{ 0.0f };
		uint32_t mFirstId{ 0 };
		Texture* mTexture{ nullptr };
		std::unordered_map<uint32_t, std::unordered_map<std::string, 
			std::string>> mProperties;
	};

	class TileSetEditor : public IEditor
	{
	public:

		TileSetEditor() = default;
		virtual ~TileSetEditor() = default;

		TileSetEditor(const TileSetEditor&) = delete;
		TileSetEditor& operator = (const TileSetEditor&) = delete;

		TileSetEditor(TileSetEditor&&) = default;
		TileSetEditor& operator = (TileSetEditor&&) = default;

		virtual void setTileSet(TileSet& tileSet);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		TileSet* mTileSet{ nullptr };
		std::string mSelectedTextureFile;
	};

	class TileSetSerializer : public ISerializer
	{
	public:

		TileSetSerializer() = default;
		virtual ~TileSetSerializer() = default;

		TileSetSerializer(const TileSetSerializer&) = delete;
		TileSetSerializer& operator = (const TileSetSerializer&) = delete;

		TileSetSerializer(TileSetSerializer&&) = default;
		TileSetSerializer& operator = (TileSetSerializer&&) = default;

		virtual void setTileSet(TileSet& tileSet);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		TileSet* mTileSet{ nullptr };
	};
}