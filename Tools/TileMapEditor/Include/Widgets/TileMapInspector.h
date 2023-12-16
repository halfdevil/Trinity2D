#pragma once

#include "Widgets/Inspector.h"

namespace Trinity
{
	class TileMap;
	struct TileMapFrame;
	struct TileMapAnimation;

	class TileMapInspector : public Inspector
	{
	public:

		TileMapInspector() = default;
		virtual ~TileMapInspector() = default;

		TileMapInspector(const TileMapInspector&) = delete;
		TileMapInspector& operator = (const TileMapInspector&) = delete;

		TileMapInspector(TileMapInspector&&) = default;
		TileMapInspector& operator = (TileMapInspector&&) = default;

		virtual void setTileMap(TileMap& tileMap);
		virtual void setSelectedTileSet(uint32_t selectedTileSet);

	protected:

		virtual void drawInspector() override;

	protected:

		TileMap* mTileMap{ nullptr };
		uint32_t mSelectedTileSet{ 0 };
		std::string mTileMapName;
	};
}