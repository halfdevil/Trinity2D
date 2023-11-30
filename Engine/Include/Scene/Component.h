#pragma once

#include "Editor/Editor.h"
#include "VFS/Serializer.h"
#include <memory>
#include <string>
#include <typeindex>
#include <vector>
#include "uuid_v4.h"

namespace Trinity
{
	class Node;
	class Scene;
	class ComponentSerializer;
	class ComponentEditor;

	class Component
	{
	public:

		friend class ComponentSerializer;
		friend class ComponentEditor;

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

		bool isActive() const
		{
			return mActive;
		}

		virtual IEditor* getEditor(Scene& scene);
		virtual ISerializer* getSerializer(Scene& scene);

		virtual void setName(const std::string& name);
		virtual void setActive(bool active);
		virtual void setNode(Node& node);

		virtual std::type_index getType() const = 0;
		virtual UUIDv4::UUID getTypeUUID() const = 0;

	protected:

		std::string mName;
		bool mActive{ true };
		Node* mNode{ nullptr };
	};

	class ComponentEditor : public IEditor
	{
	public:

		ComponentEditor() = default;
		virtual ~ComponentEditor() = default;

		ComponentEditor(const ComponentEditor&) = delete;
		ComponentEditor& operator = (const ComponentEditor&) = delete;

		ComponentEditor(ComponentEditor&&) = default;
		ComponentEditor& operator = (ComponentEditor&&) = default;

		virtual void setComponent(Component& component);
		virtual void setScene(Scene& scene);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		virtual void addCommonFields(const IEditorLayout& layout);

	protected:

		Component* mComponent{ nullptr };
		Scene* mScene{ nullptr };
	};

	class ComponentSerializer : public ISerializer
	{
	public:

		ComponentSerializer() = default;
		virtual ~ComponentSerializer() = default;

		ComponentSerializer(const ComponentSerializer&) = delete;
		ComponentSerializer& operator = (const ComponentSerializer&) = delete;

		ComponentSerializer(ComponentSerializer&&) = default;
		ComponentSerializer& operator = (ComponentSerializer&&) = default;

		virtual void setComponent(Component& component);
		virtual void setScene(Scene& scene);

		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		Component* mComponent{ nullptr };
		Scene* mScene{ nullptr };
	};
}