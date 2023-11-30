#include "Scene/QuadTree.h"
#include <stack>

namespace Trinity
{
	void QuadTree::create(const BoundingRect& minBounds, const BoundingRect& bounds)
	{
		mMinBounds = minBounds;
		mBounds = bounds;

		auto root = std::make_unique<QuadTreeNode>();
		root->bounds = bounds;

		mRoot = root.get();
		mNodes.push_back(std::move(root));

		std::stack<QuadTreeNode*> traverseNodes;
		traverseNodes.push(mRoot);

		auto minSize = minBounds.getSize();
		while (!traverseNodes.empty())
		{
			auto* node = traverseNodes.top();
			traverseNodes.pop();

			auto min = node->bounds.min;
			auto max = node->bounds.max;
			auto size = node->bounds.getSize();
			auto center = node->bounds.getCenter();

			if ((size.x * 0.5f) >= minSize.x && ((size.y * 0.5f) >= minSize.y))
			{
				std::vector<BoundingRect> childBounds = {
					{ glm::vec2(min.x, min.y), glm::vec2(center.x, center.y) },
					{ glm::vec2(center.x, min.y), glm::vec2(max.x, center.y) },
					{ glm::vec2(center.x, center.y), glm::vec2(max.x, max.y) },
					{ glm::vec2(min.x, center.y), glm::vec2(center.x, max.y) }
				};

				for (uint32_t idx = 0; idx < 4; idx++)
				{
					auto childNode = std::make_unique<QuadTreeNode>();
					childNode->bounds = childBounds[idx];

					traverseNodes.push(childNode.get());
					node->children.push_back(childNode.get());

					mNodes.push_back(std::move(childNode));
				}
			}
		}
	}

	bool QuadTree::insert(QuadTreeData& data)
	{
		std::stack<QuadTreeNode*> traverseNodes;
		traverseNodes.push(mRoot);

		while (!traverseNodes.empty())
		{
			auto* node = traverseNodes.top();
			traverseNodes.pop();

			if (node->bounds.isIntersecting(data.bounds))
			{
				if (node->isLeaf())
				{
					node->contents.push_back(&data);
					return true;
				}
				else
				{
					for (auto* child : node->children)
					{
						traverseNodes.push(child);
					}
				}
			}
		}

		return false;
	}

	void QuadTree::remove(QuadTreeData& data)
	{
		std::stack<QuadTreeNode*> traverseNodes;
		traverseNodes.push(mRoot);

		while (!traverseNodes.empty())
		{
			auto* node = traverseNodes.top();
			traverseNodes.pop();

			if (node->bounds.isIntersecting(data.bounds))
			{
				if (node->isLeaf())
				{
					auto& contents = node->contents;
					if (auto it = std::find(contents.begin(), contents.end(), &data); it != contents.end())
					{
						contents.erase(it);
					}
				}
				else
				{
					for (auto* child : node->children)
					{
						traverseNodes.push(child);
					}
				}
			}
		}
	}

	void QuadTree::update(QuadTreeData& data)
	{
		remove(data);
		insert(data);
	}

	void QuadTree::query(const BoundingRect& area, std::vector<QuadTreeData*>& result)
	{
		result.clear();

		std::stack<QuadTreeNode*> traverseNodes;
		traverseNodes.push(mRoot);

		while (!traverseNodes.empty())
		{
			auto* node = traverseNodes.top();
			traverseNodes.pop();

			if (node->bounds.isIntersecting(area))
			{
				if (node->isLeaf())
				{
					for (auto* data : node->contents)
					{
						result.push_back(data);
					}
				}
				else
				{
					for (auto* child : node->children)
					{
						traverseNodes.push(child);
					}
				}
			}
		}
	}
}