#include "Widgets/TileMapInspector.h"
#include "TileMap/TileMap.h"
#include "TileMap/TileLayer.h"
#include "TileMap/TileSet.h"
#include "Editor/Editor.h"
#include "Core/EditorLayout.h"
#include "Core/EditorTheme.h"
#include "misc/cpp/imgui_stdlib.h"

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
		}
	}
}