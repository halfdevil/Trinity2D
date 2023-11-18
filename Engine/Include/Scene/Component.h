#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <vector>

namespace Trinity
{
	class Node;
	class Editor;

	class Component
	{
	public:

		Component() = default;
		virtual ~Component() = default;

		Component(const Component&) = delete;
		Component& operator = (const Component&) = delete;

		Component(Component&&) = default;
		Component& operator = (Component&&) = default;

		const std::string& getName() const
		{
			return mName;
		}

		Node* getNode() const
		{
			return mNode;
		}

		virtual Editor* getEditor();
		virtual void setName(const std::string& name);
		virtual void setNode(Node& node);

		virtual std::type_index getType() const = 0;
		virtual std::string getTypeName() const = 0;

	protected:

		std::string mName;
		Node* mNode{ nullptr };
	};
}