#include "Widgets/SceneHierarchy.h"
#include "Widgets/ContextMenu.h"
#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "IconsFontAwesome6.h"

namespace Trinity
{
	SceneHierarchy::SceneHierarchy()
	{
		mNodeContextMenu = std::make_unique<ContextMenu>();
		mNodeContextMenu->setTitle("##node-contextMenu");
		mNodeContextMenu->onMenuItemClick.subscribe([this](const auto& menuItem) {
			onMenuItemClicked(menuItem);
		});

		mNodeContextMenu->addMenuItem("cut", "  Cut  ");
		mNodeContextMenu->addMenuItem("copy", "  Copy  ");
		mNodeContextMenu->addMenuItem("paste", "  Paste  ");
		mNodeContextMenu->addSeparator();
		mNodeContextMenu->addMenuItem("rename", "  Rename  ");
		mNodeContextMenu->addMenuItem("duplicate", "  Duplicate  ");
		mNodeContextMenu->addMenuItem("delete", "  Delete  ");
		mNodeContextMenu->addSeparator();
		mNodeContextMenu->addMenuItem("createEmpty", "  Create Empty  ");
		
		auto* object2D = mNodeContextMenu->addMenuItem("object2D", "  2D Object  ");
		mNodeContextMenu->addMenuItem("textureRenderable", "  Texture Renderable  ", "", object2D);

		mNodeContextMenu->addMenuItem("effects", "  Effects  ");
		mNodeContextMenu->addMenuItem("light", "  Light  ");
		mNodeContextMenu->addMenuItem("audio", "  Audio  ");
	}

	void SceneHierarchy::setScene(Scene& scene)
	{
		mScene = &scene;
		mCurrentNode = mScene->getRoot();
	}

	void SceneHierarchy::draw()
	{
		if (!isEnabled())
		{
			return;
		}

		ImGui::Begin(mTitle.c_str(), &mEnabled);
		{
			if (mScene != nullptr && mScene->getRoot() != nullptr)
			{
				drawNode(mScene->getRoot(), false, true);
			}

			ImGui::End();
		}
	}

	void SceneHierarchy::drawNode(Node* node, bool menuOpened, bool defaultOpen)
	{
		auto flags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_SpanFullWidth;

		if (defaultOpen)
		{
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

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
		if (ImGui::IsItemClicked() || (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiPopupFlags_MouseButtonRight)))
		{
			mCurrentNode = node;
			onSelectNode.notify(mCurrentNode);
		}

		if (!menuOpened)
		{
			mNodeContextMenu->draw();
			menuOpened = mNodeContextMenu->isOpen();
		}

		if (hasOpen)
		{
			for (auto* child : children)
			{
				if (child != nullptr)
				{
					drawNode(child, menuOpened);
				}
			}

			ImGui::TreePop();
		}
	}

	void SceneHierarchy::onMenuItemClicked(const MenuItem& menuItem)
	{
		if (mCurrentNode != nullptr)
		{
			if (menuItem.name == "createEmpty")
			{
				mScene->addEmpty(glm::vec3{ 0.0f }, glm::vec3{ 0.0f }, mCurrentNode);
			}
		}
	}
}