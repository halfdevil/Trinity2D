#pragma once

#include "Widgets/Viewport.h"
#include "Core/Observer.h"

namespace Trinity
{
	class TileSet;
	class Texture;

	class TileSetViewport : public Viewport
	{
	public:

		const glm::vec4 kSelectionColor{ 1.0f, 0.6f, 0.0f, 0.5f };
		const glm::vec4 kTileGridColor{ 0.2f, 0.2f, 0.2f, 1.0f };

		struct SelectionData
		{
			bool selecting{ false };
			glm::vec2 start{ 0.0f };
			glm::vec2 end{ 0.0f };
		};

		TileSetViewport() = default;
		virtual ~TileSetViewport() = default;

		TileSetViewport(const TileSetViewport&) = delete;
		TileSetViewport& operator = (const TileSetViewport&) = delete;

		TileSetViewport(TileSetViewport&&) = default;
		TileSetViewport& operator = (TileSetViewport&&) = default;

		uint32_t getSelectedTile() const
		{
			return mSelectedTile;
		}

		virtual bool create(EditorResources& resources);
		virtual void destroy() override;
		virtual void drawContent(float deltaTime) override;
		virtual void setTileSet(TileSet* tileSet);

	protected:

		virtual void drawTileSet();
		virtual void drawTileGrid();
		virtual void drawSelection();
		virtual void selectTiles();

		virtual void onMousePositionUpdated(float x, float y) override;
		virtual void onMouseButtonStateUpdated(int32_t button, bool pressed) override;
		virtual void onViewportResize(uint32_t width, uint32_t height) override;
		virtual glm::vec2 convertToViewport(const glm::vec2& v) const override;

		virtual glm::ivec2 getFirstTileIndex(const glm::ivec2& minTileIdx,
			const glm::ivec2& maxTileIdx) const;

	public:

		Observer<const glm::ivec2&, const glm::ivec2&, const glm::ivec2&> onSelectTiles;

	protected:

		TileSet* mTileSet{ nullptr };
		Texture* mCurrentTexture{ nullptr };
		uint32_t mSelectedTile{ 0 };
		glm::uvec2 mViewportSize{ 0 }; 
		bool mMouseButtonDown{ false };
		SelectionData mSelectionData;
	};
}