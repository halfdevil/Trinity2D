#pragma once

#include "Core/EditorWidget.h"
#include "Widgets/Menu.h"
#include <string>
#include <memory>

namespace Trinity
{
	class Scene;
	class Node;
	class ContextMenu;

	class SceneHierarchy : public EditorWidget
	{
	public:

		SceneHierarchy();
		virtual ~SceneHierarchy() = default;

		SceneHierarchy(const SceneHierarchy&) = delete;
		SceneHierarchy& operator = (const SceneHierarchy&) = delete;

		SceneHierarchy(SceneHierarchy&&) = default;
		SceneHierarchy& operator = (SceneHierarchy&&) = default;

		Node* getCurrentNode() const
		{
			return mCurrentNode;
		}

		virtual void setScene(Scene& scene);
		virtual void draw() override;

	protected:

		virtual void drawNode(Node* node, bool menuOpened);
		virtual void onMenuItemClicked(const MenuItem& menuItem);

	protected:

		Scene* mScene{ nullptr };
		Node* mCurrentNode{ nullptr };
		std::unique_ptr<ContextMenu> mNodeContextMenu{ nullptr };
	};
}