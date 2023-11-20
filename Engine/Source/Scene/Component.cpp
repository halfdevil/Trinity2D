#include "Scene/Component.h"
#include "Scene/Node.h"
#include "Scene/Scene.h"
#include "Editor/Editor.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/Logger.h"

namespace Trinity
{
    Editor* Component::getEditor()
    {
        return nullptr;
    }

    Serializer* Component::getSerializer(Scene& scene)
    {
        return nullptr;
    }

    void Component::setName(const std::string& name)
    {
        mName = name;
    }

    void Component::setNode(Node& node)
    {
        mNode = &node;
    }

    void ComponentSerializer::setComponent(Component& component)
    {
        mComponent = &component;
    }

    void ComponentSerializer::setScene(Scene& scene)
    {
        mScene = &scene;
    }

    bool ComponentSerializer::read(FileReader& reader, ResourceCache& cache)
    {
        auto name = reader.readString();
        auto nodeUUID = reader.readString();

        if (!nodeUUID.empty())
        {
            mComponent->mNode = mScene->findNode(UUIDv4::UUID::fromStrFactory(nodeUUID));
            mComponent->mNode->setComponent(*mComponent);
        }

        mComponent->mName = name;
        return true;
    }

    bool ComponentSerializer::write(FileWriter& writer)
    {
        if (!writer.writeString(mComponent->mName))
        {
            LogError("FileWriter::writeString() failed for node name");
            return false;
        }

        std::string nodeUUID;
        if (mComponent->mNode != nullptr)
        {
            nodeUUID = mComponent->mNode->getUUID().str();
        }
        
        if (!writer.writeString(nodeUUID))
        {
            LogError("FileWriter::writeString() failed for node UUID");
            return false;
        }

        return true;
    }

    bool ComponentSerializer::read(json& object, ResourceCache& cache)
    {
        if (!object.contains("name"))
        {
            LogError("JSON component object doesn't have name key");
            return false;
        }

        std::string nodeUUID;
        if (object.contains("node"))
        {
            nodeUUID = object["node"].get<std::string>();
        }

        auto name = object["name"].get<std::string>();
        if (!nodeUUID.empty())
        {
            mComponent->mNode = mScene->findNode(UUIDv4::UUID::fromStrFactory(nodeUUID));
            mComponent->mNode->setComponent(*mComponent);
        }

        mComponent->mName = name;
        return true;
    }

    bool ComponentSerializer::write(json& object)
    {
        auto uuid = mComponent->getUUID().str();
        auto name = mComponent->mName;
        
        std::string nodeUUID;
        if (mComponent->mNode != nullptr)
        {
            nodeUUID = mComponent->mNode->getUUID().str();
        }

        object["uuid"] = uuid;
        object["name"] = name;
        object["node"] = nodeUUID;

        return true;
    }
}