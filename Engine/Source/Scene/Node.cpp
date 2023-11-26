#include "Scene/Node.h"
#include "Scene/Scene.h"
#include "Scene/Components/Script.h"
#include "Scene/Components/ScriptContainer.h"
#include "Editor/EditorLayout.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/Logger.h"

namespace Trinity
{
	Node::Node()
	{
		static UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
		mUUID = uuidGenerator.getUUID();

		mTransform.setNode(*this);
		mScriptContainer.setNode(*this);

		setComponent(mTransform);
		setComponent(mScriptContainer);
	}

	ISerializer* Node::getSerializer(Scene& scene)
	{
		static NodeSerializer serializer;
		serializer.setNode(*this);
		serializer.setScene(scene);

		return &serializer;
	}

	void Node::setName(const std::string& name)
	{
		mName = name;
	}

	void Node::setUUID(const UUIDv4::UUID& uuid)
	{
		mUUID = uuid;
	}

	void Node::setParent(Node& parent)
	{
		mParent = &parent;
		mTransform.invalidateWorldMatrix();
	}

	void Node::addChild(Node& child)
	{
		child.setParent(*this);
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
		component.setNode(*this);

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

	void NodeEditor::setScene(Scene& scene)
	{
		mScene = &scene;
	}

	void NodeEditor::setNode(Node& node)
	{
		mNode = &node;
	}

	void NodeEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{
		auto& transform = mNode->mTransform;
		if (auto* editor = transform.getEditor(*mScene); editor != nullptr)
		{
			editor->onInspectorGui(layout, cache);
		}

		for (auto& it : mNode->mComponents)
		{
			if (it.first == typeid(Transform) || it.first == typeid(ScriptContainer))
			{
				continue;
			}

			if (auto* editor = it.second->getEditor(*mScene); editor != nullptr)
			{
				editor->onInspectorGui(layout, cache);
			}
		}
	}

	void NodeSerializer::setNode(Node& node)
	{
		mNode = &node;
	}

	void NodeSerializer::setScene(Scene& scene)
	{
		mScene = &scene;
	}

	bool NodeSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		auto uuid = UUIDv4::UUID::fromStrFactory(reader.readString());
		auto name = reader.readString();

		mScene->updateNodeUUID(mNode, uuid);
		mNode->mName = name;

		auto parentUUID = reader.readString();
		if (!parentUUID.empty())
		{
			auto* parent = mScene->findNode(UUIDv4::UUID::fromStrFactory(parentUUID));
			if (!parent)
			{
				LogError("Unable to find parent in scene with uuid: '%s'", parentUUID.c_str());
				return false;
			}
			else
			{
				parent->addChild(*mNode);
			}
		}

		if (auto* serializer = mNode->mTransform.getSerializer(*mScene); serializer != nullptr)
		{
			if (!serializer->read(reader, cache))
			{
				LogError("TransformSerializer::read() failed for node: '%s'", mNode->getName().c_str());
				return false;
			}
		}

		if (auto* serializer = mNode->mScriptContainer.getSerializer(*mScene); serializer != nullptr)
		{
			if (!serializer->read(reader, cache))
			{
				LogError("ScriptContainerSerializer::read() failed for node: '%s'", mNode->getName().c_str());
				return false;
			}
		}

		return true;
	}

	bool NodeSerializer::write(FileWriter& writer)
	{
		if (!writer.writeString(mNode->mUUID.str()))
		{
			LogError("FileWriter::writeString() failed for node 'uuid'");
			return false;
		}

		if (!writer.writeString(mNode->mName))
		{
			LogError("FileWriter::writeString() failed for node 'uuid'");
			return false;
		}

		if (mNode->mParent != nullptr)
		{
			if (!writer.writeString(mNode->mParent->mUUID.str()))
			{
				LogError("FileWriter::write() failed for node 'parent'");
				return false;
			}
		}

		if (auto* serializer = mNode->mTransform.getSerializer(*mScene); serializer != nullptr)
		{
			if (!serializer->write(writer))
			{
				LogError("TransformSerializer::write() failed for node: '%s'", mNode->getName().c_str());
				return false;
			}
		}

		if (auto* serializer = mNode->mScriptContainer.getSerializer(*mScene); serializer != nullptr)
		{
			if (!serializer->write(writer))
			{
				LogError("ScriptContainerSerializer::write() failed for node: '%s'", mNode->getName().c_str());
				return false;
			}
		}

		return true;
	}

	bool NodeSerializer::read(json& object, ResourceCache& cache)
	{
		if (!object.contains("uuid"))
		{
			LogError("JSON Node object doesn't contain 'uuid' key");
			return false;
		}

		if (!object.contains("name"))
		{
			LogError("JSON Node object doesn't contain 'name' key");
			return false;
		}

		if (!object.contains("parent"))
		{
			LogError("JSON Node object doesn't contain 'parent' key");
			return false;
		}

		auto uuid = UUIDv4::UUID::fromStrFactory(object["uuid"].get<std::string>());
		auto name = object["name"].get<std::string>();

		mScene->updateNodeUUID(mNode, uuid);
		mNode->mName = name;

		auto parentUUID = object["parent"].get<std::string>();
		if (!parentUUID.empty())
		{
			auto* parent = mScene->findNode(UUIDv4::UUID::fromStrFactory(parentUUID));
			if (!parent)
			{
				LogError("Unable to find parent in scene with uuid: '%s'", parentUUID.c_str());
				return false;
			}
			else
			{
				parent->addChild(*mNode);
			}
		}

		if (object.contains("transform"))
		{
			if (auto* serializer = mNode->mTransform.getSerializer(*mScene); serializer != nullptr)
			{
				if (!serializer->read(object["transform"], cache))
				{
					LogError("TransformSerializer::read() failed for node: '%s'", mNode->getName().c_str());
					return false;
				}
			}
		}

		if (object.contains("scriptContainer"))
		{
			if (auto* serializer = mNode->mScriptContainer.getSerializer(*mScene); serializer != nullptr)
			{
				if (!serializer->read(object["scriptContainer"], cache))
				{
					LogError("ScriptContainerSerializer::read() failed for node: '%s'", mNode->getName().c_str());
					return false;
				}
			}
		}

		return true;
	}

	bool NodeSerializer::write(json& object)
	{
		std::string parentUUID;
		if (mNode->mParent != nullptr)
		{
			parentUUID = mNode->mParent->mUUID.str();
		}

		object["uuid"] = mNode->mUUID.str();
		object["name"] = mNode->mName;
		object["parent"] = parentUUID;

		if (auto* serializer = mNode->mTransform.getSerializer(*mScene); serializer != nullptr)
		{
			object["transform"] = {};
			if (!serializer->write(object["transform"]))
			{
				LogError("TransformSerializer::write() failed for node: '%s'", mNode->getName().c_str());
				return false;
			}
		}

		if (auto* serializer = mNode->mScriptContainer.getSerializer(*mScene); serializer != nullptr)
		{
			object["scriptContainer"] = {};
			if (!serializer->write(object["scriptContainer"]))
			{
				LogError("ScriptContainerSerializer::write() failed for node: '%s'", mNode->getName().c_str());
				return false;
			}
		}

		return true;
	}
}