#pragma once

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
	class Editor;
	class ComponentSerializer;

	class Component
	{
	public:

		friend class ComponentSerializer;

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
		virtual Serializer* getSerializer(Scene& scene);

		virtual void setName(const std::string& name);
		virtual void setNode(Node& node);

		virtual std::type_index getType() const = 0;
		virtual UUIDv4::UUID getUUID() const = 0;

	protected:

		std::string mName;
		Node* mNode{ nullptr };
	};

	class ComponentSerializer : public Serializer
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