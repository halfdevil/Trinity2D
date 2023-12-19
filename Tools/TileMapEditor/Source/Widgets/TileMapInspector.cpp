#include "Widgets/TileMapInspector.h"
#include "TileMap/TileMap.h"
#include "TileMap/TileLayer.h"
#include "TileMap/TileSet.h"
#include "Editor/Editor.h"
#include "Core/EditorLayout.h"
#include "Core/EditorTheme.h"
#include "misc/cpp/imgui_stdlib.h"
#include "IconsFontAwesome6.h"

namespace Trinity
{
	void TileMapInspector::setTileMap(TileMap& tileMap)
	{
		mTileMap = &tileMap;
		mTileMapName = tileMap.getName();
	}

	void TileMapInspector::setSelectedTileSet(uint32_t selectedTileSet)
	{
		mSelectedTileSet = selectedTileSet;
	}

	void TileMapInspector::setSelectedTile(const glm::ivec2& selectedTile)
	{
		mSelectedTile = selectedTile;
	}

	void TileMapInspector::drawPropertyTable(const std::unordered_map<std::string, std::string>& properties, PropertyEdit& propertyEdit)
	{
		const float tableHeight = ImGui::GetTextLineHeightWithSpacing() * 7.25f +
			ImGui::GetStyle().FramePadding.y * 2.0f;

		auto tableFlags = ImGuiTableFlags_Resizable |
			ImGuiTableFlags_Borders |
			ImGuiTableFlags_ScrollY;

		if (ImGui::BeginTable("##propertyEdit", 2, tableFlags, ImVec2{ 0.0f, tableHeight }))
		{
			bool isEditing{ false };

			for (auto& [key, value] : properties)
			{
				auto selected = propertyEdit.selected == key;

				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();

				if (propertyEdit.edited == key)
				{
					if (ImGui::InputText("##propertyName", &propertyEdit.name, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						isEditing = true;
					}
				}
				else
				{
					if (ImGui::Selectable(key.c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns))
					{
						propertyEdit.selected = key;
						if (propertyEdit.name != propertyEdit.selected)
						{
							propertyEdit.name.clear();
						}
					}
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					propertyEdit.edited = key;
					propertyEdit.name = key;
					propertyEdit.value = value;
				}

				ImGui::TableSetColumnIndex(1);
				ImGui::AlignTextToFramePadding();

				if (propertyEdit.edited == key)
				{
					if (ImGui::InputText("##propertyValue", &propertyEdit.value, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						isEditing = true;
					}
				}
				else
				{
					ImGui::TextUnformatted(value.c_str());
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					propertyEdit.edited = key;
					propertyEdit.name = key;
					propertyEdit.value = value;
				}
			}

			if (isEditing)
			{
				onPropertyEditComplete(propertyEdit);
			}

			ImGui::EndTable();
		}

		ImGui::PushID("##operation");

		if (ImGui::Button(ICON_FA_PLUS))
		{
			onAddProperty(propertyEdit);
		}

		if (!properties.empty())
		{
			ImGui::SameLine();

			if (ImGui::Button(ICON_FA_TRASH))
			{
				onRemoveProperty(propertyEdit);
			}

			ImGui::SameLine();

			if (ImGui::Button(ICON_FA_PENCIL))
			{
				onEditProperty(propertyEdit);
			}
		}

		ImGui::PopID();
	}

	void TileMapInspector::drawInspector()
	{
		if (mTileMap != nullptr)
		{
			ImGui::TextUnformatted("Name: ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

			if (ImGui::InputText("##name", &mTileMapName))
			{
				mTileMap->setName(mTileMapName);
			}

			ImGui::Separator();

			if (auto* editor = mTileMap->getEditor(mSelectedTileSet); editor != nullptr)
			{
				editor->onInspectorGui(*mLayout, *mResourceCache);
			}

			if (mSelectedTile.x != -1 && mSelectedTile.y != -1)
			{
				const float tableHeight = ImGui::GetTextLineHeightWithSpacing() * 7.25f +
					ImGui::GetStyle().FramePadding.y * 2.0f;

				if (auto* tileSet = mTileMap->getTileSet(mSelectedTileSet); tileSet != nullptr)
				{
					const auto& numTiles = tileSet->getNumTiles();
					auto tileIndex = mSelectedTile.y * (int32_t)numTiles.x + mSelectedTile.x;

					if (ImGui::CollapsingHeader("Selected Tile", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::SetNextItemWidth(-FLT_MIN);

						if (tileSet->hasProperties((uint32_t)tileIndex))
						{
							drawPropertyTable(tileSet->getProperties((uint32_t)tileIndex), mTilePropertyEdit);
						}
						else
						{
							drawPropertyTable({}, mTilePropertyEdit);
						}
					}
				}
			}
		}
	}

	void TileMapInspector::onPropertyEditComplete(PropertyEdit& propertyEdit)
	{
		if (propertyEdit.type == PropertyType::Tile)
		{
			if (auto* tileSet = mTileMap->getTileSet(mSelectedTileSet); tileSet != nullptr)
			{
				const auto& numTiles = tileSet->getNumTiles();
				auto index = mSelectedTile.y * (int32_t)numTiles.x + mSelectedTile.x;

				tileSet->removeProperty(index, propertyEdit.edited);
				tileSet->addProperty(index, propertyEdit.name, propertyEdit.value);

				propertyEdit.edited.clear();
				propertyEdit.name.clear();
				propertyEdit.value.clear();
			}
		}
	}

	void TileMapInspector::onAddProperty(PropertyEdit& propertyEdit)
	{
		if (propertyEdit.type == PropertyType::Tile)
		{
			if (auto* tileSet = mTileMap->getTileSet(mSelectedTileSet); tileSet != nullptr)
			{
				const auto& numTiles = tileSet->getNumTiles();
				auto index = mSelectedTile.y * (int32_t)numTiles.x + mSelectedTile.x;

				tileSet->addProperty(index, "Name", "Value");
			}
		}
	}

	void TileMapInspector::onRemoveProperty(PropertyEdit& propertyEdit)
	{
		if (propertyEdit.type == PropertyType::Tile)
		{
			if (auto* tileSet = mTileMap->getTileSet(mSelectedTileSet); tileSet != nullptr)
			{
				const auto& numTiles = tileSet->getNumTiles();
				auto index = mSelectedTile.y * (int32_t)numTiles.x + mSelectedTile.x;

				tileSet->removeProperty(index, propertyEdit.selected);
			}
		}
	}

	void TileMapInspector::onEditProperty(PropertyEdit& propertyEdit)
	{
		if (propertyEdit.type == PropertyType::Tile)
		{
			if (auto* tileSet = mTileMap->getTileSet(mSelectedTileSet); tileSet != nullptr)
			{
				const auto& numTiles = tileSet->getNumTiles();
				auto index = mSelectedTile.y * (int32_t)numTiles.x + mSelectedTile.x;

				const auto& properties = tileSet->getProperties(index);
				if (auto it = properties.find(propertyEdit.selected); it != properties.end())
				{
					propertyEdit.edited = propertyEdit.selected;
					propertyEdit.name = propertyEdit.selected;
					propertyEdit.value = it->second;
				}
			}
		}
	}
}