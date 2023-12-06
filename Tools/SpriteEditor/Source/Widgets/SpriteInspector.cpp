#include "Widgets/SpriteInspector.h"
#include "Scene/Sprite.h"
#include "Editor/Editor.h"
#include "Core/EditorLayout.h"
#include "Core/EditorTheme.h"
#include "misc/cpp/imgui_stdlib.h"

namespace Trinity
{
	void SpriteInspector::setSprite(Sprite& sprite)
	{
		mSprite = &sprite;
		mSpriteName = sprite.getName();
	}

	void SpriteInspector::setSelectedFrame(uint32_t selectedFrame)
	{
		mSelectedFrame = selectedFrame;
	}

	void SpriteInspector::setSelectedAnimation(uint32_t selectedAnimation)
	{
		mSelectedAnimation = selectedAnimation;
	}

	void SpriteInspector::drawInspector()
	{
		if (mSprite != nullptr)
		{
			ImGui::TextUnformatted("Name: ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

			if (ImGui::InputText("##name", &mSpriteName))
			{
				mSprite->setName(mSpriteName);
			}

			ImGui::Separator();

			if (auto* editor = mSprite->getEditor(mSelectedFrame, mSelectedAnimation); editor != nullptr)
			{
				editor->onInspectorGui(*mLayout, *mResourceCache);
			}
		}
	}
}