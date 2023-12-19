#pragma once

#include "Widgets/Viewport.h"
#include "Core/Observer.h"
#include <queue>

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

		static constexpr uint32_t kMaxNumUndo = 24;
		const glm::vec4 kSelectionColor{ 1.0f, 0.6f, 0.0f, 0.5f };
		const glm::vec4 kTileGridColor{ 0.2f, 0.2f, 0.2f, 1.0f };

		struct SelectionData
		{
			bool valid{ false };
			bool selecting{ false };
			uint32_t layer{ 0 };
			glm::vec2 start{ 0.0f };
			glm::vec2 end{ 0.0f };
		};

		struct SelectedTiles
		{
			glm::ivec2 minTileIndex{ 0 };
			glm::ivec2 maxTileIndex{ 0 };
			glm::ivec2 firstTileIndex{ 0 };
		};

		struct EditBuffer
		{
			uint32_t layer{ 0 };
			std::unordered_map<uint32_t, uint32_t> tiles;
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

		virtual void setSelectedTiles(const glm::ivec2& minTileIndex, const glm::ivec2& maxTileIndex, 
			const glm::ivec2& firstTileIndex);

		virtual void copy();
		virtual void cut();
		virtual void paste();
		virtual void undo();
		virtual void redo();

	protected:

		virtual void drawTileMap();
		virtual void drawTileGrid();
		virtual void drawSelection();
		virtual void bucketFillTiles(const glm::ivec2& tileIndex);
		virtual void paintTiles(const glm::ivec2& tileIndex);
		virtual void bucketFill(const glm::uvec2& tileIndex, uint32_t tileId);
		virtual void drawTopToolbarWidgets() override;
		
		virtual void onMousePositionUpdated(float x, float y) override;
		virtual void onMouseButtonStateUpdated(int32_t button, bool pressed) override;
		virtual void onViewportResize(uint32_t width, uint32_t height) override;
		virtual glm::vec2 convertToViewport(const glm::vec2& v) const override;

	public:

		Observer<> onTileMapEdit;

	protected:

		TileMap* mTileMap{ nullptr };
		TileMapEditType mEditType{ TileMapEditType::Paint };
		bool mShowTileGrid{ true };
		bool mMouseButtonDown{ false };
		uint32_t mSelectedTileLayer{ 0 };
		uint32_t mSelectedTileSet{ 0 };
		glm::ivec2 mLastTileIndex{ -1 };
		SelectionData mSelectionData;
		SelectedTiles mSelectedTiles;
		EditBuffer mCopiedBuffer;
		std::deque<EditBuffer> mUndoQueue;
		std::deque<EditBuffer> mRedoQueue;
	};
}