#include "Scene/Component.h"
#include "Scene/Node.h"
#include "Editor/Editor.h"

namespace Trinity
{
    Editor* Component::getEditor()
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

    void Component::onGui()
    {
    }
}