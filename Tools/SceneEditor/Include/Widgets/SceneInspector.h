#pragma once

#include "Widgets/Inspector.h"

namespace Trinity
{
	class SceneInspector : public Inspector
	{
	public:

		SceneInspector();
		virtual ~SceneInspector() = default;

		SceneInspector(const SceneInspector&) = delete;
		SceneInspector& operator = (const SceneInspector&) = delete;

		SceneInspector(SceneInspector&&) = default;
		SceneInspector& operator = (SceneInspector&&) = default;

		virtual void setScene(Scene& scene);
		virtual void setSelectedNode(Node* node);

	protected:

		virtual void drawInspector() override;

	protected:

		std::unique_ptr<NodeEditor> mEditor{ nullptr };
		Scene* mScene{ nullptr };
		Node* mSelectedNode{ nullptr };
		std::string mSelectedNodeName;
	};
}