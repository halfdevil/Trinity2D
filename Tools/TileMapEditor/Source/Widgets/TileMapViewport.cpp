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

	void TileMapViewport::setSelectedTile(uint32_t selectedIdx)
	{
		mSelectedTile = selectedIdx;
	}

	void TileMapViewport::copySelection()
	{
		if (mSelectionData.hasSelection)
		{
			mCopiedSelectionData = mSelectionData;
		}
	}

	void TileMapViewport::pasteSelection()
	{
		if (mCopiedSelectionData.hasSelection && mSelectionData.hasSelection)
		{
			const auto& numTiles = mTileMap->getNumTiles();
			const auto& tileSize = mTileMap->getTileSize();
			const auto& size = mTileMap->getSize();
			const auto position = size * -0.5f;

			const auto minPos = glm::min(mCopiedSelectionData.start, mCopiedSelectionData.end);
			const auto maxPos = glm::max(mCopiedSelectionData.start, mCopiedSelectionData.end);

			const auto minTileIdx = glm::ivec2{
				(int32_t)((minPos.x - position.x) / tileSize.x),
				(int32_t)((minPos.y - position.y) / tileSize.y)
			};

			const auto maxTileIdx = glm::ivec2{
				(int32_t)((maxPos.x - position.x) / tileSize.x),
				(int32_t)((maxPos.y - position.y) / tileSize.y)
			};

			const auto pastePosition = mSelectionData.start;
			auto tileIndex = glm::uvec2{
				(uint32_t)((pastePosition.x - position.x) / tileSize.x),
				(uint32_t)((pastePosition.y - position.y) / tileSize.y)
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

					auto destTileIdx = glm::uvec2{
						tileIndex.x + jdx - minTileIdx.x,
						tileIndex.y + idx - minTileIdx.y
					};

					if (destTileIdx.x < 0 || destTileIdx.x >= numTiles.x ||
						destTileIdx.y < 0 || destTileIdx.y >= numTiles.y)
					{
						continue;
					}

					auto tileId = mTileMap->getTile(mCopiedSelectionData.selectedLayer, (uint32_t)jdx, (uint32_t)idx);
					mTileMap->setTile(mSelectedTileLayer, destTileIdx.x, destTileIdx.y, tileId);
				}
			}
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
		const auto& size = mTileMap->getSize();
		const auto position = size * -0.5f;

		auto& input = Input::get();
		auto* mouse = input.getMouse();
		bool isSelected = mMouseInViewport && mouse->isButtonDown(MOUSE_BUTTON_LEFT);

		if (isSelected)
		{
			if (mEditType != TileMapEditType::Selection)
			{
				auto finalPosition = convertToViewport(mouse->getPosition());

				if ((finalPosition.x >= position.x && finalPosition.x < position.x + size.x) &&
					(finalPosition.y >= position.y && finalPosition.y < position.y + size.y))
				{
					auto tileIndex = glm::uvec2{
						(uint32_t)((finalPosition.x - position.x) / tileSize.x),
						(uint32_t)((finalPosition.y - position.y) / tileSize.y)
					};

					if (mEditType == TileMapEditType::BucketFill)
					{
						auto* tileSet = mTileMap->getTileSet(mSelectedTileSet);
						auto tileId = tileSet->getFirstId() + mSelectedTile;

						bucketFill(tileIndex, tileId);
					}
					else
					{
						auto* tileSet = mTileMap->getTileSet(mSelectedTileSet);
						if (tileSet != nullptr && tileSet->getTexture() != nullptr)
						{
							auto tileId = mEditType == TileMapEditType::Paint ?
								tileSet->getFirstId() + mSelectedTile : 0U;

							mTileMap->setTile(mSelectedTileLayer, tileIndex.x, tileIndex.y, tileId);
						}
					}
				}
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

				mLineCanvas->line(from, to, glm::vec4{ 0.2f, 0.2f, 0.2f, 1.0f });
			}

			for (uint32_t idx = 0; idx <= numTiles.y; idx++)
			{
				const auto from = glm::vec2{ 0.0f, idx * tileSize.y } + position;
				const auto to = glm::vec2{ size.x, idx * tileSize.y } + position;

				mLineCanvas->line(from, to, glm::vec4{ 0.2f, 0.2f, 0.2f, 1.0f });
			}
		}
	}

	void TileMapViewport::drawSelection()
	{
		if (!mSelectionData.hasSelection)
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
				mRenderer->drawRect(tilePos, tileSize, glm::vec2{ 0.0f }, glm::mat4{ 1.0f }, 
					glm::vec4{ 1.0f, 0.6f, 0.0f, 0.3f });
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

		std::queue<glm::uvec2> fillQueue;
		fillQueue.push(tileIndex);

		const auto& numTiles = mTileMap->getNumTiles();
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
				mTileMap->setTile(mSelectedTileLayer, posX + 1, posY, tileId);
				fillQueue.push({ posX + 1, posY });
			}

			if (isValid(posX - 1, posY, oldTileId, tileId))
			{
				mTileMap->setTile(mSelectedTileLayer, posX - 1, posY, tileId);
				fillQueue.push({ posX - 1, posY });
			}

			if (isValid(posX, posY + 1, oldTileId, tileId))
			{
				mTileMap->setTile(mSelectedTileLayer, posX, posY + 1, tileId);
				fillQueue.push({ posX, posY + 1 });
			}

			if (isValid(posX, posY - 1, oldTileId, tileId))
			{
				mTileMap->setTile(mSelectedTileLayer, posX, posY - 1, tileId);
				fillQueue.push({ posX, posY - 1 });
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

		if (EditorHelper::toggleButton(ICON_FA_SQUARE, mEditType == TileMapEditType::Selection))
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

		if (mSelectionData.isSelecting)
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
							.hasSelection = true,
							.isSelecting = true,
							.selectedLayer = mSelectedTileLayer,
							.start = convertToViewport(mouse->getPosition()),
							.end = convertToViewport(mouse->getPosition())
						};
					}
					else
					{
						mSelectionData.isSelecting = false;
						mSelectionData.end = convertToViewport(mouse->getPosition());
					}
				}
				else
				{
					mSelectionData.isSelecting = false;
				}
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