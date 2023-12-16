#include "Widgets/TileSetViewport.h"
#include "TileMap/TileSet.h"
#include "Input/Input.h"
#include "Math/BoundingRect.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderPass.h"
#include "Graphics/LineCanvas.h"
#include "Core/EditorTheme.h"
#include "Core/EditorResources.h"
#include "Core/EditorGizmo.h"
#include "Core/EditorCamera.h"
#include "Core/EditorGrid.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "Utils/EditorHelper.h"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	bool TileSetViewport::create(EditorResources& resources)
	{
		if (!Viewport::create(resources, 4))
		{
			LogError("Viewport::create() failed");
			return false;
		}

		mKeepAspectRatio = false;
		mShowTopToolbar = false;
		mShowBottomToolbar = false;
		mShowGizmoControls = false;
		mShowResolutionControls = false;

		mViewportSize = {
			mSelectedResolution->width,
			mSelectedResolution->height
		};

		return true;
	}

	void TileSetViewport::destroy()
	{
		Viewport::destroy();
		mTileSet = nullptr;
	}

	void TileSetViewport::setTileSet(TileSet* tileSet)
	{
		mTileSet = tileSet;
		if (mTileSet != nullptr)
		{
			setSelectedTile(0);
		}
	}

	void TileSetViewport::setSelectedTile(uint32_t selectedTile)
	{
		mSelectedTile = selectedTile;
		onSelectTile.notify(mSelectedTile);
	}

	void TileSetViewport::drawContent(float deltaTime)
	{
		if (mTileSet != nullptr)
		{
			if (auto* texture = mTileSet->getTexture(); texture != nullptr)
			{
				if (mCurrentTexture != texture)
				{
					mCurrentTexture = texture;
					resize(texture->getWidth(), texture->getHeight());
				}
			}
			else
			{
				mCurrentTexture = nullptr;
			}
		}

		mRenderPass->begin(*mFrameBuffer);

		if (mGrid != nullptr)
		{
			mGrid->draw(*mRenderPass);
		}

		if (mCamera != nullptr)
		{
			mRenderer->begin(mCamera->getViewProj());

			if (mTileSet != nullptr)
			{
				drawTileSet();
				drawSelection();
			}

			mRenderer->end(*mRenderPass);
			mLineCanvas->draw(mCamera->getViewProj(), *mRenderPass);
		}

		mRenderPass->end();
	}

	void TileSetViewport::drawTileSet()
	{
		if (mCurrentTexture != nullptr)
		{
			const auto size = glm::vec2{
				(float)mCurrentTexture->getWidth(),
				(float)mCurrentTexture->getHeight()
			};

			mRenderer->drawTexture(mCurrentTexture, glm::vec2{ 0.0f }, size, 
				glm::vec2{ 0.0f}, size);
		}
	}

	void TileSetViewport::drawSelection()
	{
		const auto& tileSize = mTileSet->getTileSize();
		const auto& numTiles = mTileSet->getNumTiles();

		auto& input = Input::get();
		auto* mouse = input.getMouse();
		bool isSelected = mMouseInViewport && mouse->isButtonTriggered(MOUSE_BUTTON_LEFT);

		if (isSelected)
		{
			auto finalPosition = convertToViewport(mouse->getPosition());
			for (uint32_t idx = 0; idx < numTiles.x * numTiles.y; idx++)
			{
				const auto row = idx / numTiles.x;
				const auto col = idx % numTiles.x;

				const auto position = glm::vec2{
					col * tileSize.x,
					row * tileSize.y
				};

				if ((finalPosition.x >= position.x && finalPosition.x < position.x + tileSize.x) &&
					(finalPosition.y >= position.y && finalPosition.y < position.y + tileSize.y))
				{
					if (mSelectedTile != idx)
					{
						setSelectedTile(idx);
					}
				}
			}
		}

		if (numTiles.x > 0 && numTiles.y > 0)
		{
			const auto row = mSelectedTile / numTiles.x;
			const auto col = mSelectedTile % numTiles.x;

			const auto min = glm::vec2{
				col * tileSize.x,
				row * tileSize.y
			};

			const auto max = min + tileSize;
			const auto rect = BoundingRect{	min, max };

			mLineCanvas->rect(rect, glm::vec4{ 1.0f, 0.6f, 0.0f, 1.0f }, 2.0f);
		}
	}

	void TileSetViewport::onViewportResize(uint32_t width, uint32_t height)
	{
		Viewport::onViewportResize(width, height);

		if (mGrid != nullptr)
		{
			mGrid->setCanvasSize(glm::vec2{ (float)width, (float)height });
			mGrid->updateGridData();
		}
	}

	glm::vec2 TileSetViewport::convertToViewport(const glm::vec2& v) const
	{
		const auto& cameraSize = mCamera->getSize();
		const auto& transform = mCamera->getTransform();

		glm::vec2 scale{
			cameraSize.x / mSize.x,
			cameraSize.y / mSize.y
		};

		auto scaledValue = ((v - mPosition) * scale);
		return glm::vec2(transform * glm::vec4{ scaledValue.x, cameraSize.y - scaledValue.y, 0.0f, 1.0f });
	}
}