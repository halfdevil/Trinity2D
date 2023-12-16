#pragma once

#include "Widgets/Viewport.h"

namespace Trinity
{
	class TileMap;
	class TileLayer;
	class TileSet;

	enum class TileMapEditType
	{
		Paint,
		BucketFill,
		Eraser,
		Selection
	};

	class TileMapViewport : public Viewport
	{
	public:

		struct SelectionData
		{
			bool hasSelection{ false };
			bool isSelecting{ false };
			uint32_t selectedLayer{ 0 };
			glm::vec2 start{ 0.0f };
			glm::vec2 end{ 0.0f };
		};

		TileMapViewport() = default;
		virtual ~TileMapViewport() = default;

		TileMapViewport(const TileMapViewport&) = delete;
		TileMapViewport& operator = (const TileMapViewport&) = delete;

		TileMapViewport(TileMapViewport&&) = default;
		TileMapViewport& operator = (TileMapViewport&&) = default;

		virtual bool create(EditorResources& resources);
		virtual void destroy() override;
		virtual void drawContent(float deltaTime) override;

		virtual void setTileMap(TileMap& tileMap);
		virtual void setSelectedTileLayer(uint32_t selectedIdx);
		virtual void setSelectedTileSet(uint32_t selectedIdx);
		virtual void setSelectedTile(uint32_t selectedIdx);
		virtual void copySelection();
		virtual void pasteSelection();

	protected:

		virtual void drawTileMap();
		virtual void drawTileGrid();
		virtual void drawSelection();
		virtual void bucketFill(const glm::uvec2& tileIndex, uint32_t tileId);
		virtual void drawTopToolbarWidgets() override;
		
		virtual void onMousePositionUpdated(float x, float y) override;
		virtual void onMouseButtonStateUpdated(int32_t button, bool pressed) override;
		virtual void onViewportResize(uint32_t width, uint32_t height) override;
		virtual glm::vec2 convertToViewport(const glm::vec2& v) const override;

	protected:

		TileMap* mTileMap{ nullptr };
		TileMapEditType mEditType{ TileMapEditType::Paint };
		bool mShowTileGrid{ true };
		bool mMouseButtonDown{ false };
		uint32_t mSelectedTileLayer{ 0 };
		uint32_t mSelectedTileSet{ 0 };
		uint32_t mSelectedTile{ 0 };
		SelectionData mSelectionData;
		SelectionData mCopiedSelectionData;
	};
}