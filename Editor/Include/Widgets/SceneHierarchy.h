#pragma once

#include "Core/EditorWidget.h"
#include <string>

namespace Trinity
{
	class Scene;
	class Node;

	class SceneHierarchy : public EditorWidget
	{
	public:

		SceneHierarchy() = default;
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

		virtual void drawNode(Node* node);

	protected:

		Scene* mScene{ nullptr };
		Node* mCurrentNode{ nullptr };
	};
}