#pragma once

#include "Core/EditorWidget.h"
#include "Core/Observer.h"
#include <string>
#include <memory>

namespace Trinity
{
	class TileMap;
	class TileSet;
	class TileLayer;

	class TileMapHierarchy : public EditorWidget
	{
	public:

		static constexpr float kBaseVisibilityListBoxWidth = 20.0f;

		TileMapHierarchy() = default;
		virtual ~TileMapHierarchy() = default;

		TileMapHierarchy(const TileMapHierarchy&) = delete;
		TileMapHierarchy& operator = (const TileMapHierarchy&) = delete;

		TileMapHierarchy(TileMapHierarchy&&) = default;
		TileMapHierarchy& operator = (TileMapHierarchy&&) = default;

		uint32_t getSelectedTileSet() const
		{
			return mSelectedTileSet;
		}

		uint32_t getSelectedTileLayer() const
		{
			return mSelectedTileLayer;
		}

		virtual void setTileMap(TileMap& tileMap);
		virtual void setSelectedTileSet(uint32_t selectedTileSet);
		virtual void setSelectedTileLayer(uint32_t selectedTileLayer);
		virtual void draw() override;

	public:

		Observer<uint32_t> onSelectTileSet;
		Observer<uint32_t> onSelectTileLayer;

	protected:

		TileMap* mTileMap{ nullptr };
		uint32_t mSelectedTileSet{ 0 };
		uint32_t mSelectedTileLayer{ 0 };
	};
}