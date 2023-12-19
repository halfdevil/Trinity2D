#pragma once

#include "Core/EditorApp.h"

namespace Trinity
{
	class TileMap;
	class TileSet;
	class TileMapViewport;
	class TileMapInspector;
	class TileMapHierarchy;
	class TileSetViewport;

	class TileMapEditorApp : public EditorApp
	{
	public:

		TileMapEditorApp() = default;
		virtual ~TileMapEditorApp() = default;

		TileMapEditorApp(const TileMapEditorApp&) = delete;
		TileMapEditorApp& operator = (const TileMapEditorApp&) = delete;

		TileMapEditorApp(TileMapEditorApp&&) = default;
		TileMapEditorApp& operator = (TileMapEditorApp&&) = default;

	protected:

		virtual bool init() override;
		virtual void setupInput() override;

		virtual void onUpdate(float deltaTime) override;
		virtual void onDraw(float deltaTime) override;

		virtual TileMap* openTileMap(const std::string& path);
		virtual bool saveTileMap(TileMap* tileMap, const std::string& path);
		virtual void updateTitle();

		virtual MenuBar* createMainMenu() override;
		virtual AssetFileDialog* createFileDialog() override;

		virtual TileMap* createDefaultTileMap() const;
		virtual TileMapViewport* createTileMapViewport(const std::string& title, TileMap& tileMap);
		virtual TileMapInspector* createTileMapInspector(const std::string& title, TileMap& tileMap);
		virtual TileMapHierarchy* createTileMapHierarchy(const std::string& title, TileMap& tileMap);
		virtual TileSetViewport* createTileSetViewport(const std::string& tile);

		virtual void onMainMenuClick(const std::string& name) override;
		virtual void onAssetFileDialogClick(AssetFileDialogType dialogType, AssetFileDialogResult result,
			const std::string& path) override;

		virtual void onKeyboardKeyStateUpdated(int32_t key, bool pressed);
		virtual void onViewportResize(uint32_t width, uint32_t height);
		virtual void onTileSetViewportResize(uint32_t width, uint32_t height);

		virtual void onSelectTilesClick(const glm::ivec2& minTileIndex, const glm::ivec2& maxTileIndex, 
			const glm::ivec2& firstTileIndex);

		virtual void onSelectTileLayerClick(uint32_t selectedTileLayer);
		virtual void onSelectTileSetClick(uint32_t selectedTileSet);
		virtual void onTileMapEditClick();

	protected:

		TileMap* mCurrentTileMap{ nullptr };
		TileMapViewport* mViewport{ nullptr };
		TileMapInspector* mInspector{ nullptr };
		TileMapHierarchy* mHierarchy{ nullptr };
		TileSetViewport* mTileSetViewport{ nullptr };
		bool mTileMapChanged{ true };
	};
}