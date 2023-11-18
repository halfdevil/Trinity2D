#pragma once

#include "Scene/Components/Transform.h"
#include "Scene/Components/ScriptContainer.h"
#include "Editor/Editor.h"
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Trinity
{
	class NodeEditor;

	class Node
	{
	public:

		friend class NodeEditor;

		Node();
		virtual ~Node() = default;

		Node(const Node&) = delete;
		Node& operator = (const Node&) = delete;

		Node(Node&&) = default;
		Node& operator = (Node&&) = default;

		const std::string& getName() const
		{
			return mName;
		}

		Transform& getTransform()
		{
			return mTransform;
		}

		ScriptContainer& getScriptContainer()
		{
			return mScriptContainer;
		}

		Node* getParent() const
		{
			return mParent;
		}

		const std::vector<Node*>& getChildren() const
		{
			return mChildren;
		}

		virtual void setName(const std::string& name);
		virtual void setParent(Node& parent);
		virtual void addChild(Node& child);

		virtual Component& getComponent(const std::type_index& type);
		virtual bool hasComponent(const std::type_index& type);
		virtual void setComponent(Component& component);

	public:

		template <typename T>
		inline T& getComponent()
		{
			return dynamic_cast<T&>(getComponent(typeid(T)));
		}

		template <typename T>
		inline bool hasComponent()
		{
			return hasComponent(typeid(T));
		}

	protected:

		std::string mName;
		Node* mParent{ nullptr };
		Transform mTransform;
		ScriptContainer mScriptContainer;
		std::vector<Node*> mChildren;
		std::unordered_map<std::type_index, Component*> mComponents;
	};

	class NodeEditor : public Editor
	{
	public:

		NodeEditor() = default;
		virtual ~NodeEditor() = default;

		virtual void setNode(Node& node);
		virtual void onInspectorGui(const EditorLayout& layout) override;

	protected:

		Node* mNode{ nullptr };
	};
}