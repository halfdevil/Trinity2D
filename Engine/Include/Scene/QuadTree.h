#pragma once

#include "Math/BoundingRect.h"
#include <vector>
#include <memory>

namespace Trinity
{
	struct QuadTreeData
	{
		BoundingRect bounds;		
	};

	struct QuadTreeNode
	{
		bool isLeaf()
		{
			return children.size() == 0;
		}

		BoundingRect bounds;
		std::vector<QuadTreeNode*> children;
		std::vector<QuadTreeData*> contents;
	};

	class QuadTree
	{
	public:

		QuadTree() = default;
		virtual ~QuadTree() = default;

		QuadTree(const QuadTree&) = delete;
		QuadTree& operator = (const QuadTree&) = delete;

		QuadTree(QuadTree&&) = default;
		QuadTree& operator = (QuadTree&&) = default;

		virtual void create(const BoundingRect& minBounds, const BoundingRect& bounds);
		virtual bool insert(QuadTreeData& data);
		virtual void remove(QuadTreeData& data);

		virtual void update(QuadTreeData& data);
		virtual void query(const BoundingRect& area, std::vector<QuadTreeData*>& result);

	protected:

		BoundingRect mMinBounds;
		BoundingRect mBounds;
		QuadTreeNode* mRoot{ nullptr };
		std::vector<std::unique_ptr<QuadTreeNode>> mNodes;
	};
}
