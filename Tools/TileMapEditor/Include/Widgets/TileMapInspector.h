#pragma once

#include "Widgets/Inspector.h"
#include "glm/glm.hpp"
#include <unordered_map>

namespace Trinity
{
	class TileMap;
	struct TileMapFrame;
	struct TileMapAnimation;

	class TileMapInspector : public Inspector
	{
	public:

		enum class PropertyType
		{
			Tile,
			Object
		};

		struct PropertyEdit
		{
			PropertyType type{ PropertyType::Tile };
			std::string selected;
			std::string edited;
			std::string name;
			std::string value;
		};

		TileMapInspector() = default;
		virtual ~TileMapInspector() = default;

		TileMapInspector(const TileMapInspector&) = delete;
		TileMapInspector& operator = (const TileMapInspector&) = delete;

		TileMapInspector(TileMapInspector&&) = default;
		TileMapInspector& operator = (TileMapInspector&&) = default;

		virtual void setTileMap(TileMap& tileMap);
		virtual void setSelectedTileSet(uint32_t selectedTileSet);	
		virtual void setSelectedTile(const glm::ivec2& selectedTile);

	protected:

		virtual void drawPropertyTable(const std::unordered_map<std::string, std::string>& properties, 
			PropertyEdit& propertyEdit);

		virtual void drawInspector() override;
		virtual void onPropertyEditComplete(PropertyEdit& propertyEdit);
		virtual void onAddProperty(PropertyEdit& propertyEdit);
		virtual void onRemoveProperty(PropertyEdit& propertyEdit);
		virtual void onEditProperty(PropertyEdit& propertyEdit);

	protected:

		TileMap* mTileMap{ nullptr };
		uint32_t mSelectedTileSet{ 0 };
		glm::ivec2 mSelectedTile{ 0 };
		std::string mTileMapName;
		PropertyEdit mTilePropertyEdit;
		PropertyEdit mObjectPropertyEdit;
	};
}