#pragma once

#include "Core/EditorApp.h"

namespace Trinity
{
	class TileMap;

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

	protected:

		TileMap* mCurrentTileMap{ nullptr };
	};
}