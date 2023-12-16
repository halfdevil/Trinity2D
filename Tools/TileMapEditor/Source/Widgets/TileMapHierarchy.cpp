#include "Widgets/TileMapHierarchy.h"
#include "Core/EditorTheme.h"
#include "TileMap/TileMap.h"
#include "TileMap/TileLayer.h"
#include "TileMap/TileSet.h"
#include "Utils/EditorHelper.h"
#include <format>
#include "IconsFontAwesome6.h"
#include "imgui_internal.h"

namespace Trinity
{
	void TileMapHierarchy::setTileMap(TileMap& tileMap)
	{
		mTileMap = &tileMap;
	}

	void TileMapHierarchy::setSelectedTileSet(uint32_t selectedTileSet)
	{
		mSelectedTileSet = selectedTileSet;
	}

	void TileMapHierarchy::setSelectedTileLayer(uint32_t selectedTileLayer)
	{
		mSelectedTileLayer = selectedTileLayer;
	}

	void TileMapHierarchy::draw()
	{
		if (!isEnabled())
		{
			return;
		}

		ImGui::Begin(mTitle.c_str(), &mEnabled);
		{
			ImGui::SetNextItemWidth(-FLT_MIN);

			const float tableHeight = ImGui::GetTextLineHeightWithSpacing() * 7.25f + 
				ImGui::GetStyle().FramePadding.y * 2.0f;

			const auto& tileLayers = mTileMap->getTileLayers();
			const auto& tileSets = mTileMap->getTileSets();

			if (ImGui::CollapsingHeader("Tile Layers", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SetNextItemWidth(-FLT_MIN);

				if (ImGui::BeginTable("##tileLayers", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2{ 0.0f, tableHeight }))
				{
					for (uint32_t idx = 0; idx < (uint32_t)tileLayers.size(); idx++)
					{
						auto* tileLayer = tileLayers[idx];

						ImGui::SetNextItemWidth(-FLT_MIN);
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::AlignTextToFramePadding();

						bool selected = mSelectedTileLayer == idx;
						if (ImGui::Selectable(tileLayer->getName().c_str(), &selected))
						{
							if (idx != mSelectedTileLayer)
							{
								mSelectedTileLayer = idx;
								onSelectTileLayer.notify(mSelectedTileLayer);
							}
						}

						ImGui::TableSetColumnIndex(1);
						ImGui::PushID(idx);

						if (ImGui::Selectable(tileLayer->isVisible() ? ICON_FA_EYE : ICON_FA_EYE_SLASH, &selected))
						{
							tileLayer->toggleVisibility();
						}

						ImGui::PopID();
					}

					ImGui::EndTable();
				}

				auto operation = EditorHelper::listBoxEditor("##tileLayer-editor");
				switch (operation)
				{
				case ListBoxOperation::Add:
				{
					auto tileLayer = std::make_unique<TileLayer>();
					tileLayer->setName(std::format("tileLayer{}", (uint32_t)tileLayers.size()));
					mTileMap->addTileLayer(std::move(tileLayer));

					if (mTileMap->getTileLayers().size() == 1)
					{
						onSelectTileLayer.notify(0);
					}

					break;
				}

				case ListBoxOperation::Delete:
				{
					if (tileLayers.size() > 0)
					{
						mTileMap->removeTileLayer(mSelectedTileLayer);
						if (mSelectedTileLayer > 0)
						{
							mSelectedTileLayer--;
						}

						onSelectTileLayer.notify(mSelectedTileLayer);
					}
					break;
				}

				case ListBoxOperation::Up:
				{
					if (mSelectedTileLayer > 0)
					{
						mTileMap->moveTileLayer(mSelectedTileLayer, mSelectedTileLayer - 1);
						mSelectedTileLayer--;
					}
					break;
				}

				case ListBoxOperation::Down:
				{
					if (mSelectedTileLayer < (uint32_t)tileLayers.size() - 1)
					{
						mTileMap->moveTileLayer(mSelectedTileLayer, mSelectedTileLayer + 1);
						mSelectedTileLayer++;
					}
					break;
				}

				default:
					break;
				}
			}

			ImGui::SetNextItemWidth(-FLT_MIN);

			if (ImGui::CollapsingHeader("Tile Sets", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SetNextItemWidth(-FLT_MIN);

				if (ImGui::BeginTable("##tileSets", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2{ 0.0f, tableHeight }))
				{
					for (uint32_t idx = 0; idx < (uint32_t)tileSets.size(); idx++)
					{
						ImGui::SetNextItemWidth(-FLT_MIN);
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::AlignTextToFramePadding();

						bool selected = mSelectedTileSet == idx;
						if (ImGui::Selectable(tileSets[idx]->getName().c_str(), &selected))
						{
							if (idx != mSelectedTileSet)
							{
								mSelectedTileSet = idx;
								onSelectTileSet.notify(mSelectedTileSet);
							}
						}
					}

					ImGui::EndTable();
				}

				auto operation = EditorHelper::listBoxEditor("##tileSet-editor");
				switch (operation)
				{
				case ListBoxOperation::Add:
				{
					auto numTileSets = (uint32_t)tileSets.size();
					auto tileSet = std::make_unique<TileSet>();

					tileSet->setName(std::format("tileSet{}", numTileSets));
					tileSet->setFirstId((mTileMap->getTotalTiles() * numTileSets) + 1);
					mTileMap->addTileSet(std::move(tileSet));

					if (mTileMap->getTileSets().size() == 1)
					{
						onSelectTileSet.notify(0);
					}

					break;
				}

				case ListBoxOperation::Delete:
				{
					if (tileSets.size() > 0)
					{
						mTileMap->removeTileSet(mSelectedTileSet);
						if (mSelectedTileSet > 0)
						{
							mSelectedTileSet--;
						}

						onSelectTileSet.notify(mSelectedTileSet);
					}
					break;
				}

				case ListBoxOperation::Up:
				{
					if (mSelectedTileSet > 0)
					{
						mTileMap->moveTileSet(mSelectedTileSet, mSelectedTileSet - 1);
						mSelectedTileSet--;
					}
					break;
				}

				case ListBoxOperation::Down:
				{
					if (mSelectedTileSet < (uint32_t)tileSets.size() - 1)
					{
						mTileMap->moveTileSet(mSelectedTileSet, mSelectedTileSet + 1);
						mSelectedTileSet++;
					}
					break;
				}

				default:
					break;
				}
			}

			ImGui::End();
		}
	}
}