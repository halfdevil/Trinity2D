#pragma once

#include "Core/EditorWidget.h"
#include <string>
#include <memory>
#include <vector>

namespace Trinity
{
	class Scene;
	class Node;
	class NodeEditor;
	class ResourceCache;
	class EditorLayout;

	class Inspector : public EditorWidget
	{
	public:

		Inspector();
		virtual ~Inspector() = default;

		Inspector(const Inspector&) = delete;
		Inspector& operator = (const Inspector&) = delete;

		Inspector(Inspector&&) = default;
		Inspector& operator = (Inspector&&) = default;

		virtual void setLayout(EditorLayout& layout);
		virtual void setResourceCache(ResourceCache& cache);
		virtual void setScene(Scene& scene);
		virtual void setSelectedNode(Node* node);
		virtual void draw() override;

	protected:

		virtual void addComponent();

	protected:

		EditorLayout* mLayout{ nullptr };
		ResourceCache* mResourceCache{ nullptr };
		std::unique_ptr<NodeEditor> mEditor{ nullptr };
		Scene* mScene{ nullptr };
		Node* mSelectedNode{ nullptr };
		std::string mSelectedComponentName;
		std::vector<std::string> mComponentNames;
		std::string mSelectedNodeName;
		bool mIsOpen{ true };
	};
}