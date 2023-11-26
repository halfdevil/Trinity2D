#pragma once

#include "Scene/Components/Transform.h"
#include "Scene/Components/ScriptContainer.h"
#include "Editor/Editor.h"
#include "VFS/Serializer.h"
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include "uuid_v4.h"

namespace Trinity
{
	class NodeEditor;
	class NodeSerializer;
	class Scene;

	class Node
	{
	public:

		friend class NodeEditor;
		friend class NodeSerializer;

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

		UUIDv4::UUID getUUID() const
		{
			return mUUID;
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

		ISerializer* getSerializer(Scene& scene);

		virtual void setName(const std::string& name);
		virtual void setUUID(const UUIDv4::UUID& uuid);
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
		UUIDv4::UUID mUUID;
		Node* mParent{ nullptr };
		Transform mTransform;
		ScriptContainer mScriptContainer;
		std::vector<Node*> mChildren;
		std::unordered_map<std::type_index, Component*> mComponents;
	};

	class NodeEditor : public IEditor
	{
	public:

		NodeEditor() = default;
		virtual ~NodeEditor() = default;

		virtual void setScene(Scene& scene);
		virtual void setNode(Node& node);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		Scene* mScene{ nullptr };
		Node* mNode{ nullptr };
	};

	class NodeSerializer : public ISerializer
	{
	public:

		NodeSerializer() = default;
		virtual ~NodeSerializer() = default;

		NodeSerializer(const NodeSerializer&) = delete;
		NodeSerializer& operator = (const NodeSerializer&) = delete;

		NodeSerializer(NodeSerializer&&) = default;
		NodeSerializer& operator = (NodeSerializer&&) = default;

		virtual void setNode(Node& node);
		virtual void setScene(Scene& scene);

		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		Node* mNode{ nullptr };
		Scene* mScene{ nullptr };
	};
}