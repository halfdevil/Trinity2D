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

		mKeepAspectRatio = true;
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
		mSelectionData = {};
		selectTiles();
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

			if (mTileSet != nullptr)
			{
				drawTileGrid();
			}

			mLineCanvas->draw(mCamera->getViewProj(), *mRenderPass);
		}

		mRenderPass->end();
	}

	void TileSetViewport::drawTileSet()
	{
		if (mCurrentTexture != nullptr)
		{
			const auto& size = mTileSet->getSize();
			mRenderer->drawTexture(mCurrentTexture, glm::vec2{ 0.0f }, size, 
				size * -0.5f, size);
		}
	}

	void TileSetViewport::drawTileGrid()
	{
		const auto& tileSize = mTileSet->getTileSize();
		const auto& numTiles = mTileSet->getNumTiles();
		const auto& size = mTileSet->getSize();
		const auto position = size * -0.5f;

		if (numTiles.x > 0 && numTiles.y > 0)
		{
			for (uint32_t idx = 0; idx <= numTiles.x; idx++)
			{
				const auto from = glm::vec2{ idx * tileSize.x, 0.0f } + position;
				const auto to = glm::vec2{ idx * tileSize.x, size.y } + position;

				mLineCanvas->line(from, to, kTileGridColor);
			}

			for (uint32_t idx = 0; idx <= numTiles.y; idx++)
			{
				const auto from = glm::vec2{ 0.0f, idx * tileSize.y } + position;
				const auto to = glm::vec2{ size.x, idx * tileSize.y } + position;

				mLineCanvas->line(from, to, kTileGridColor);
			}
		}
	}

	void TileSetViewport::drawSelection()
	{
		const auto& numTiles = mTileSet->getNumTiles();
		const auto& tileSize = mTileSet->getTileSize();
		const auto& size = mTileSet->getSize();
		const auto position = size * -0.5f;

		const auto minPos = glm::min(mSelectionData.start, mSelectionData.end);
		const auto maxPos = glm::max(mSelectionData.start, mSelectionData.end);

		const auto minTileIdx = glm::ivec2{
			(int32_t)((minPos.x - position.x) / tileSize.x),
			(int32_t)((minPos.y - position.y) / tileSize.y)
		};

		const auto maxTileIdx = glm::ivec2{
			(int32_t)((maxPos.x - position.x) / tileSize.x),
			(int32_t)((maxPos.y - position.y) / tileSize.y)
		};

		for (auto idx = minTileIdx.y; idx <= maxTileIdx.y; idx++)
		{
			if (idx < 0 || idx >= (int32_t)numTiles.y)
			{
				continue;
			}

			for (auto jdx = minTileIdx.x; jdx <= maxTileIdx.x; jdx++)
			{
				if (jdx < 0 || jdx >= (int32_t)numTiles.x)
				{
					continue;
				}

				const auto rect = BoundingRect{
					glm::vec2{ jdx * tileSize.x, idx * tileSize.y } + position,
					glm::vec2{ (jdx + 1) * tileSize.x, (idx + 1) * tileSize.y } + position
				};

				const auto tilePos = glm::vec2{ jdx * tileSize.x, idx * tileSize.y } + position;
				mRenderer->drawRect(tilePos, tileSize, glm::vec2{ 0.0f },
					glm::mat4{ 1.0f }, kSelectionColor);
			}
		}
	}

	void TileSetViewport::selectTiles()
	{
		if (mTileSet != nullptr)
		{
			const auto& numTiles = mTileSet->getNumTiles();
			const auto& tileSize = mTileSet->getTileSize();
			const auto& size = mTileSet->getSize();
			const auto position = size * -0.5f;

			const auto minPos = glm::min(mSelectionData.start, mSelectionData.end);
			const auto maxPos = glm::max(mSelectionData.start, mSelectionData.end);

			const auto minTileIdx = glm::ivec2{
				(int32_t)((minPos.x - position.x) / tileSize.x),
				(int32_t)((minPos.y - position.y) / tileSize.y)
			};

			const auto maxTileIdx = glm::ivec2{
				(int32_t)((maxPos.x - position.x) / tileSize.x),
				(int32_t)((maxPos.y - position.y) / tileSize.y)
			};

			onSelectTiles.notify(minTileIdx, maxTileIdx, getFirstTileIndex(minTileIdx, maxTileIdx));
		}
	}

	void TileSetViewport::onMousePositionUpdated(float x, float y)
	{
		Viewport::onMousePositionUpdated(x, y);

		if (mSelectionData.selecting)
		{
			auto& input = Input::get();
			auto* mouse = input.getMouse();

			mSelectionData.end = convertToViewport(mouse->getPosition());
		}
	}

	void TileSetViewport::onMouseButtonStateUpdated(int32_t button, bool pressed)
	{
		Viewport::onMouseButtonStateUpdated(button, pressed);

		if (button == MOUSE_BUTTON_LEFT)
		{
			mMouseButtonDown = pressed;

			if (mMouseInViewport)
			{
				auto& input = Input::get();
				auto* mouse = input.getMouse();

				if (mMouseButtonDown)
				{
					mSelectionData = {
						.selecting = true,
						.start = convertToViewport(mouse->getPosition()),
						.end = convertToViewport(mouse->getPosition())
					};
				}
				else
				{
					if (mSelectionData.selecting)
					{
						selectTiles();
					}

					mSelectionData.selecting = false;
				}
			}
			else
			{
				if (mSelectionData.selecting)
				{
					selectTiles();
				}

				mSelectionData.selecting = false;
			}
		}
	}

	void TileSetViewport::onViewportResize(uint32_t width, uint32_t height)
	{
		Viewport::onViewportResize(width, height);

		if (mCamera != nullptr)
		{
			float halfWidth = 0.5f * width;
			float halfHeight = 0.5f * height;

			mCamera->setSize(-halfWidth, halfWidth, -halfHeight, halfHeight);
		}

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

		auto scaledValue = ((v - mPosition) * scale) - cameraSize * 0.5f;
		return glm::vec2(transform * glm::vec4{ scaledValue.x, -scaledValue.y, 0.0f, 1.0f });
	}

	glm::ivec2 TileSetViewport::getFirstTileIndex(const glm::ivec2& minTileIdx, const glm::ivec2& maxTileIdx) const
	{
		glm::ivec2 firstTileIdx{ -1 };

		for (auto idx = minTileIdx.y; idx <= maxTileIdx.y; idx++)
		{
			if (idx == -1)
			{
				continue;
			}

			for (auto jdx = minTileIdx.x; jdx <= maxTileIdx.x; jdx++)
			{
				if (jdx == -1)
				{
					continue;
				}

				firstTileIdx = { jdx, idx };
				break;
			}

			if (firstTileIdx.x != -1 && firstTileIdx.y != -1)
			{
				break;
			}
		}

		return firstTileIdx;
	}
}