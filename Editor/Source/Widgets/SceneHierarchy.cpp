#include "Widgets/SceneHierarchy.h"
#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "IconsFontAwesome6.h"

namespace Trinity
{
	void SceneHierarchy::setTitle(const std::string& title)
	{
		mTitle = title;
	}

	void SceneHierarchy::setScene(Scene& scene)
	{
		mScene = &scene;
		mCurrentNode = nullptr;
	}

	void SceneHierarchy::draw()
	{
		if (ImGui::Begin(mTitle.c_str()))
		{
			if (mScene != nullptr && mScene->getRoot() != nullptr)
			{
				drawNode(mScene->getRoot());
			}

			ImGui::End();
		}
	}

	void SceneHierarchy::drawNode(Node* node)
	{
		auto flags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_SpanFullWidth;

		std::string label = ICON_FA_CUBE " ";
		label.append(node->getName());

		auto& children = node->getChildren();
		if (children.size() == 0)
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		if (node == mCurrentNode)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool hasOpen = ImGui::TreeNodeEx(node, flags, "%s", label.c_str());
		if (ImGui::IsItemClicked())
		{
			mCurrentNode = node;
		}

		if (hasOpen)
		{
			for (auto* child : children)
			{
				if (child != nullptr)
				{
					drawNode(child);
				}
			}

			ImGui::TreePop();
		}
	}
}