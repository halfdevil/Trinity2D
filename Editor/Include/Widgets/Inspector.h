#pragma once

#include "Core/EditorWidget.h"
#include <string>
#include <memory>

namespace Trinity
{
	class Node;
	class NodeEditor;
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

		virtual void setSelectedNode(Node* node);
		virtual void draw() override;

	protected:

		Node* mSelectedNode{ nullptr };
		std::unique_ptr<NodeEditor> mEditor{ nullptr };
		std::unique_ptr<EditorLayout> mLayout{ nullptr };
		bool mIsOpen{ true };
	};
}