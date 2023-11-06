#include "Scene/Node.h"
#include "Scene/Components/Script.h"
#include "Scene/Components/ScriptContainer.h"

namespace Trinity
{
	Node::Node()
	{
		mTransform.setNode(*this);
		mScriptContainer.setNode(*this);

		setComponent(mTransform);
		setComponent(mScriptContainer);
	}

	void Node::setName(const std::string& name)
	{
		mName = name;
	}

	void Node::setParent(Node& parent)
	{
		mParent = &parent;
		mTransform.invalidateWorldMatrix();
	}

	void Node::addChild(Node& child)
	{
		mChildren.push_back(&child);
	}

	Component& Node::getComponent(const std::type_index& type)
	{
		return *mComponents.at(type);
	}

	bool Node::hasComponent(const std::type_index& type)
	{
		return mComponents.contains(type);
	}

	void Node::setComponent(Component& component)
	{
		if (component.getType() == typeid(Script))
		{
			mScriptContainer.setScript(dynamic_cast<Script&>(component));
			return;
		}

		auto it = mComponents.find(component.getType());
		if (it != mComponents.end())
		{
			it->second = &component;
		}
		else
		{
			mComponents.insert(std::make_pair(component.getType(), &component));
		}
	}
}