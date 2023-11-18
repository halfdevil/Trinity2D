#include "Widgets/Inspector.h"
#include "Scene/Node.h"
#include "Editor/EditorLayout.h"
#include "Editor/Editor.h"

namespace Trinity
{
	Inspector::Inspector()
	{
		mEditor = std::make_unique<NodeEditor>();
		mLayout = std::make_unique<EditorLayout>();
	}

	void Inspector::setTitle(const std::string& title)
	{
		mTitle = title;
	}

	void Inspector::setSelectedNode(Node* node)
	{
		mSelectedNode = node;
	}

	void Inspector::draw()
	{
		if (ImGui::Begin(mTitle.c_str()))
		{
			if (mSelectedNode != nullptr)
			{
				mEditor->setNode(*mSelectedNode);
				mEditor->onInspectorGui(*mLayout);
			}

			ImGui::End();
		}
	}
}