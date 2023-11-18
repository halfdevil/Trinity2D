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

		const std::string& getTitle() const
		{
			return mTitle;
		}

		virtual void setTitle(const std::string& title);
		virtual void setSelectedNode(Node* node);
		virtual void draw() override;

	protected:

		std::string mTitle;
		Node* mSelectedNode{ nullptr };
		std::unique_ptr<NodeEditor> mEditor{ nullptr };
		std::unique_ptr<EditorLayout> mLayout{ nullptr };
	};
}