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

		virtual void setTileSet(TileSet* tileSet);
		virtual void setSelectedTile(uint32_t selectedTile);
		virtual void drawContent(float deltaTime) override;

	protected:

		virtual void drawTileSet();
		virtual void drawSelection();

		virtual void onViewportResize(uint32_t width, uint32_t height) override;
		virtual glm::vec2 convertToViewport(const glm::vec2& v) const override;

	public:

		Observer<uint32_t> onSelectTile;

	protected:

		TileSet* mTileSet{ nullptr };
		Texture* mCurrentTexture{ nullptr };
		uint32_t mSelectedTile{ 0 };
		glm::uvec2 mViewportSize{ 0 };
	};
}