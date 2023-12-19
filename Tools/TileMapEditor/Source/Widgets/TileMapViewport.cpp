#include "Widgets/TileMapViewport.h"
#include "TileMap/TileMap.h"
#include "TileMap/TileSet.h"
#include "TileMap/TileLayer.h"
#include "Input/Input.h"
#include "Math/BoundingRect.h"
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
#include "IconsFontAwesome6.h"
#include <queue>

namespace Trinity
{
	bool TileMapViewport::create(EditorResources& resources)
	{
		if (!Viewport::create(resources))
		{
			LogError("Viewport::create() failed");
			return false;
		}

		mShowTopToolbar = true;
		mShowBottomToolbar = false;
		mShowGizmoControls = false;
		mShowResolutionControls = true;

		return true;
	}

	void TileMapViewport::destroy()
	{
		Viewport::destroy();
		mTileMap = nullptr;
	}

	void TileMapViewport::setTileMap(TileMap& tileMap)
	{
		mTileMap = &tileMap;
	}

	void TileMapViewport::setSelectedTileLayer(uint32_t selectedIdx)
	{
		mSelectedTileLayer = selectedIdx;
	}

	void TileMapViewport::setSelectedTileSet(uint32_t selectedIdx)
	{
		mSelectedTileSet = selectedIdx;
	}

	void TileMapViewport::setSelectedTiles(const glm::ivec2& minTileIndex, const glm::ivec2& maxTileIndex, 
		const glm::ivec2& firstTileIndex)
	{
		mSelectedTiles.minTileIndex = minTileIndex;
		mSelectedTiles.maxTileIndex = maxTileIndex;
		mSelectedTiles.firstTileIndex = firstTileIndex;
	}

	void TileMapViewport::copy()
	{
		if (mSelectionData.valid)
		{
			mCopiedBuffer = {
				.layer = mSelectionData.layer
			};

			const auto& numTiles = mTileMap->getNumTiles();
			const auto& tileSize = mTileMap->getTileSize();
			const auto& size = mTileMap->getSize();
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

					auto index = (idx - minTileIdx.y) * numTiles.x + (jdx - minTileIdx.x);
					auto tileId = mTileMap->getTile(mSelectionData.layer, (uint32_t)jdx, (uint32_t)idx);
					mCopiedBuffer.tiles.insert(std::make_pair(index, tileId));
				}
			}
		}
	}

	void TileMapViewport::cut()
	{
		if (mSelectionData.valid)
		{
			copy();

			const auto& numTiles = mTileMap->getNumTiles();
			const auto& tileSize = mTileMap->getTileSize();
			const auto& size = mTileMap->getSize();
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

			EditBuffer undoBuffer = {
				.layer = mSelectedTileLayer
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

					auto index = idx * numTiles.x + jdx;
					undoBuffer.tiles.insert(std::make_pair(index, mTileMap->getTile(mSelectionData.layer, index)));
					mTileMap->setTile(mSelectionData.layer, (uint32_t)jdx, (uint32_t)idx, 0);
				}
			}

			if (undoBuffer.tiles.size() != 0)
			{
				mUndoQueue.push_front(std::move(undoBuffer));
			}

			onTileMapEdit.notify();
		}
	}

	void TileMapViewport::paste()
	{
		if (mSelectionData.valid)
		{
			const auto& numTiles = mTileMap->getNumTiles();
			const auto& tileSize = mTileMap->getTileSize();
			const auto& size = mTileMap->getSize();
			const auto position = size * -0.5f;

			const auto pastePosition = mSelectionData.start;
			auto tileIndex = glm::uvec2{
				(uint32_t)((pastePosition.x - position.x) / tileSize.x),
				(uint32_t)((pastePosition.y - position.y) / tileSize.y)
			};

			EditBuffer undoBuffer = {
				.layer = mSelectedTileLayer
			};

			for (auto& [index, value] : mCopiedBuffer.tiles)
			{
				auto row = index / numTiles.x;
				auto col = index % numTiles.x;

				auto destTileIdx = glm::uvec2{
					tileIndex.x + col,
					tileIndex.y + row
				};

				auto index = destTileIdx.y * numTiles.x + destTileIdx.x;
				undoBuffer.tiles.insert(std::make_pair(index, mTileMap->getTile(mSelectedTileLayer, index)));

				mTileMap->setTile(mSelectedTileLayer, destTileIdx.x, destTileIdx.y, value);
			}

			if (undoBuffer.tiles.size() != 0)
			{
				mUndoQueue.push_front(std::move(undoBuffer));
			}

			onTileMapEdit.notify();
		}
	}

	void TileMapViewport::undo()
	{
		if (!mUndoQueue.empty())
		{
			EditBuffer redoBuffer;

			auto& undoBuffer = mUndoQueue.front();
			auto layer = undoBuffer.layer;

			for (const auto& [tileIdx, tileId] : undoBuffer.tiles)
			{
				redoBuffer.tiles.insert(std::make_pair(tileIdx, mTileMap->getTile(layer, tileIdx)));
				mTileMap->setTile(layer, tileIdx, tileId);
			}

			mRedoQueue.push_front(std::move(redoBuffer));
			mUndoQueue.pop_front();
		}
	}

	void TileMapViewport::redo()
	{
		if (!mRedoQueue.empty())
		{
			auto& redoBuffer = mRedoQueue.front();
			auto layer = redoBuffer.layer;

			for (const auto& [tileIdx, tileId] : redoBuffer.tiles)
			{
				mTileMap->setTile(layer, tileIdx, tileId);
			}

			mRedoQueue.pop_front();
		}
	}

	void TileMapViewport::drawContent(float deltaTime)
	{
		mRenderPass->begin(*mFrameBuffer);

		if (mCamera != nullptr)
		{
			mRenderer->begin(mCamera->getViewProj());

			if (mTileMap != nullptr)
			{
				drawTileMap();
				drawSelection();
			}
			
			mRenderer->end(*mRenderPass);

			if (mShowTileGrid)
			{
				if (mTileMap != nullptr)
				{
					drawTileGrid();
				}

				mLineCanvas->draw(mCamera->getViewProj(), *mRenderPass);
			}
		}

		mRenderPass->end();
	}

	void TileMapViewport::drawTileMap()
	{
		if (mTileMap->getNumTileLayers() == 0 || mTileMap->getNumTileSet() == 0)
		{
			return;
		}

		const auto& cameraSize = mCamera->getSize();
		const auto& cameraPos = mCamera->getPosition();
		const auto& tileSize = mTileMap->getTileSize();
		const auto& numTiles = mTileMap->getNumTiles();
		const auto& size = mTileMap->getSize();
		const auto position = size * -0.5f;

		auto& input = Input::get();
		auto* mouse = input.getMouse();

		if (mMouseInViewport && mMouseButtonDown && mEditType != TileMapEditType::Selection)
		{
			auto mousePosition = convertToViewport(mouse->getPosition());
			auto tileIndex = glm::ivec2{
				(int32_t)((mousePosition.x - position.x) / tileSize.x),
				(int32_t)((mousePosition.y - position.y) / tileSize.y)
			};

			if (mLastTileIndex != tileIndex)
			{
				if ((tileIndex.x >= 0 && tileIndex.x < (int32_t)numTiles.x) &&
					(tileIndex.y >= 0 && tileIndex.y < (int32_t)numTiles.y))
				{
					if (mEditType == TileMapEditType::BucketFill)
					{
						bucketFillTiles(tileIndex);
					}
					else
					{
						paintTiles(tileIndex);
					}

					onTileMapEdit.notify();
				}

				mLastTileIndex = tileIndex;
			}
		}

		float halfWidth = cameraSize.x * 0.5f;
		float halfHeight = cameraSize.y * 0.5f;

		mTileMap->draw(*mRenderer, position, -halfWidth + cameraPos.x, -halfHeight + cameraPos.y, 
			halfWidth + cameraPos.x, halfHeight + cameraPos.y);
	}

	void TileMapViewport::drawTileGrid()
	{
		const auto& tileSize = mTileMap->getTileSize();
		const auto& numTiles = mTileMap->getNumTiles();
		const auto& size = mTileMap->getSize();
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

	void TileMapViewport::drawSelection()
	{
		if (!mSelectionData.valid)
		{
			return;
		}

		const auto& numTiles = mTileMap->getNumTiles();
		const auto& tileSize = mTileMap->getTileSize();
		const auto& size = mTileMap->getSize();
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

	void TileMapViewport::bucketFillTiles(const glm::ivec2& tileIndex)
	{
		auto* tileSet = mTileMap->getTileSet(mSelectedTileSet);
		if (tileSet != nullptr && tileSet->getTexture() != nullptr)
		{
			const auto& minTileIdx = mSelectedTiles.minTileIndex;
			const auto& maxTileIdx = mSelectedTiles.maxTileIndex;
			const auto& firstTileIdx = mSelectedTiles.firstTileIndex;
			const auto& numTiles = tileSet->getNumTiles();

			auto selectedTile = firstTileIdx.y * (int32_t)numTiles.x + firstTileIdx.x;
			auto tileId = tileSet->getFirstId() + (uint32_t)selectedTile;

			bucketFill(tileIndex, tileId);
		}
	}

	void TileMapViewport::paintTiles(const glm::ivec2& tileIndex)
	{
		auto* tileSet = mTileMap->getTileSet(mSelectedTileSet);
		if (tileSet != nullptr)
		{
			const auto& numTilesInTileSet = tileSet->getNumTiles();
			const auto& numTilesInTileMap = mTileMap->getNumTiles();
			const auto& minTileIdx = mSelectedTiles.minTileIndex;
			const auto& maxTileIdx = mSelectedTiles.maxTileIndex;

			EditBuffer undoBuffer = {
				.layer = mSelectedTileLayer
			};

			for (auto idx = minTileIdx.y; idx <= maxTileIdx.y; idx++)
			{
				if (idx < 0 || idx >= (int32_t)numTilesInTileSet.y)
				{
					continue;
				}

				for (auto jdx = minTileIdx.x; jdx <= maxTileIdx.x; jdx++)
				{
					if (jdx < 0 || jdx >= (int32_t)numTilesInTileSet.x)
					{
						continue;
					}

					auto destTileIdx = glm::uvec2{
						tileIndex.x + jdx - minTileIdx.x,
						tileIndex.y + idx - minTileIdx.y
					};

					if (destTileIdx.x < 0 || destTileIdx.x >= numTilesInTileMap.x ||
						destTileIdx.y < 0 || destTileIdx.y >= numTilesInTileMap.y)
					{
						continue;
					}

					auto currTileId = mTileMap->getTile(mSelectedTileLayer, destTileIdx.x, destTileIdx.y);
					auto index = destTileIdx.y * numTilesInTileMap.x + destTileIdx.x;

					undoBuffer.tiles.insert(std::make_pair(index, currTileId));

					auto selectedTile = idx * (int32_t)numTilesInTileSet.x + jdx;
					auto tileId = mEditType == TileMapEditType::Paint ? tileSet->getFirstId() + (uint32_t)selectedTile : 0;

					mTileMap->setTile(mSelectedTileLayer, destTileIdx.x, destTileIdx.y, tileId);
				}
			}

			if (undoBuffer.tiles.size() != 0)
			{
				mUndoQueue.push_front(std::move(undoBuffer));
				if (mUndoQueue.size() >= kMaxNumUndo)
				{
					mUndoQueue.pop_back();
				}
			}
		}
	}

	void TileMapViewport::bucketFill(const glm::uvec2& tileIndex, uint32_t tileId)
	{
		auto isValid = [this](uint32_t x, uint32_t y, uint32_t oldTileId, uint32_t newTileId) 
		{
			const auto& numTiles = mTileMap->getNumTiles();	
			auto currTileId = mTileMap->getTile(mSelectedTileLayer, x, y);

			if (x < 0 || x >= numTiles.x || y < 0 || y >= numTiles.y || 
				currTileId != oldTileId || currTileId == newTileId)
			{
				return false;
			}

			return true;
		};

		EditBuffer undoBuffer = {
			.layer = mSelectedTileLayer
		};

		std::queue<glm::uvec2> fillQueue;
		fillQueue.push(tileIndex);

		const auto& numTiles = mTileMap->getNumTiles();
		auto index = tileIndex.y * numTiles.x + tileIndex.x;
		undoBuffer.tiles.insert(std::make_pair(index, mTileMap->getTile(mSelectedTileLayer, index)));

		auto oldTileId = mTileMap->getTile(mSelectedTileLayer, tileIndex.x, tileIndex.y);
		mTileMap->setTile(mSelectedTileLayer, tileIndex.x, tileIndex.y, tileId);

		while (!fillQueue.empty())
		{
			const auto& currIndex = fillQueue.front();
			fillQueue.pop();

			auto posX = currIndex.x;
			auto posY = currIndex.y;

			if (isValid(posX + 1, posY, oldTileId, tileId))
			{
				auto index = posY * numTiles.x + (posX + 1);
				undoBuffer.tiles.insert(std::make_pair(index, mTileMap->getTile(mSelectedTileLayer, index)));

				mTileMap->setTile(mSelectedTileLayer, posX + 1, posY, tileId);
				fillQueue.push({ posX + 1, posY });
			}

			if (isValid(posX - 1, posY, oldTileId, tileId))
			{
				auto index = posY * numTiles.x + (posX - 1);
				undoBuffer.tiles.insert(std::make_pair(index, mTileMap->getTile(mSelectedTileLayer, index)));

				mTileMap->setTile(mSelectedTileLayer, posX - 1, posY, tileId);
				fillQueue.push({ posX - 1, posY });
			}

			if (isValid(posX, posY + 1, oldTileId, tileId))
			{
				auto index = (posY + 1) * numTiles.x + posX;
				undoBuffer.tiles.insert(std::make_pair(index, mTileMap->getTile(mSelectedTileLayer, index)));

				mTileMap->setTile(mSelectedTileLayer, posX, posY + 1, tileId);
				fillQueue.push({ posX, posY + 1 });
			}

			if (isValid(posX, posY - 1, oldTileId, tileId))
			{
				auto index = (posY - 1) * numTiles.x + posX;
				undoBuffer.tiles.insert(std::make_pair(index, mTileMap->getTile(mSelectedTileLayer, index)));

				mTileMap->setTile(mSelectedTileLayer, posX, posY - 1, tileId);
				fillQueue.push({ posX, posY - 1 });
			}
		}

		if (undoBuffer.tiles.size() != 0)
		{
			mUndoQueue.push_front(std::move(undoBuffer));
			if (mUndoQueue.size() >= kMaxNumUndo)
			{
				mUndoQueue.pop_back();
			}
		}
	}

	void TileMapViewport::drawTopToolbarWidgets()
	{
		Viewport::drawTopToolbarWidgets();

		ImGui::BeginGroup();

		if (EditorHelper::toggleButton(ICON_FA_PAINTBRUSH, mEditType == TileMapEditType::Paint))
		{
			mEditType = TileMapEditType::Paint;	
			mSelectionData = {};
		}

		ImGui::SameLine();

		if (EditorHelper::toggleButton(ICON_FA_ERASER, mEditType == TileMapEditType::Eraser))
		{
			mEditType = TileMapEditType::Eraser;
			mSelectionData = {};
		}

		ImGui::SameLine();

		if (EditorHelper::toggleButton(ICON_FA_FILL, mEditType == TileMapEditType::BucketFill))
		{
			mEditType = TileMapEditType::BucketFill;
			mSelectionData = {};
		}

		ImGui::SameLine();

		if (EditorHelper::toggleButton(ICON_FA_VECTOR_SQUARE, mEditType == TileMapEditType::Selection))
		{
			mEditType = TileMapEditType::Selection;
		}

		ImGui::SameLine();
		ImGui::TextUnformatted("Show Grid:");
		ImGui::SameLine();

		ImGui::Checkbox("##showGrid", &mShowTileGrid);
		ImGui::EndGroup();
	}

	void TileMapViewport::onMousePositionUpdated(float x, float y)
	{
		Viewport::onMousePositionUpdated(x, y);

		if (mSelectionData.selecting)
		{
			auto& input = Input::get();
			auto* mouse = input.getMouse();

			mSelectionData.end = convertToViewport(mouse->getPosition());
		}
	}

	void TileMapViewport::onMouseButtonStateUpdated(int32_t button, bool pressed)
	{
		Viewport::onMouseButtonStateUpdated(button, pressed);

		if (button == MOUSE_BUTTON_LEFT)
		{
			mMouseButtonDown = pressed;

			if (mEditType == TileMapEditType::Selection)
			{
				if (mMouseInViewport)
				{
					auto& input = Input::get();
					auto* mouse = input.getMouse();

					if (mMouseButtonDown)
					{
						mSelectionData = {
							.valid = true,
							.selecting = true,
							.layer = mSelectedTileLayer,
							.start = convertToViewport(mouse->getPosition()),
							.end = convertToViewport(mouse->getPosition())
						};
					}
					else
					{
						mSelectionData.selecting = false;
						mSelectionData.end = convertToViewport(mouse->getPosition());
					}
				}
				else
				{
					mSelectionData.selecting = false;
				}
			}

			if (!mMouseButtonDown)
			{
				mLastTileIndex = glm::ivec2{ -1 };
			}
		}
	}

	void TileMapViewport::onViewportResize(uint32_t width, uint32_t height)
	{
		Viewport::onViewportResize(width, height);

		if (mCamera != nullptr)
		{
			float halfWidth = 0.5f * width;
			float halfHeight = 0.5f * height;

			mCamera->setSize(-halfWidth, halfWidth, -halfHeight, halfHeight);
		}
	}

	glm::vec2 TileMapViewport::convertToViewport(const glm::vec2& v) const
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
}