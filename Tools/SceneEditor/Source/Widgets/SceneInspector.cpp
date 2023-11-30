#include "Widgets/SceneInspector.h"
#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "Scene/ComponentFactory.h"
#include "Editor/Editor.h"
#include "Core/EditorLayout.h"
#include "Core/EditorTheme.h"
#include "misc/cpp/imgui_stdlib.h"

namespace Trinity
{
	SceneInspector::SceneInspector()
	{
		mEditor = std::make_unique<NodeEditor>();
	}

	void SceneInspector::setScene(Scene& scene)
	{
		mScene = &scene;
		mEditor->setScene(scene);

		setSelectedNode(mScene->getRoot());
	}

	void SceneInspector::setSelectedNode(Node* node)
	{
		mSelectedNode = node;
		if (mSelectedNode != nullptr)
		{
			mSelectedNodeName = mSelectedNode->getName();
		}
	}

	void SceneInspector::drawInspector()
	{
		if (mSelectedNode != nullptr)
		{
			ImGui::TextUnformatted("Name: ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

			if (ImGui::InputText("##name", &mSelectedNodeName))
			{
				mSelectedNode->setName(mSelectedNodeName);
			}

			ImGui::Separator();

			mEditor->setNode(*mSelectedNode);
			mEditor->onInspectorGui(*mLayout, *mResourceCache);
		}
	}
}