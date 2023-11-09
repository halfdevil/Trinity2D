#include "Scene/Component.h"
#include "Scene/Node.h"

namespace Trinity
{
	void Component::setName(const std::string& name)
    {
        mName = name;
    }

    void Component::setNode(Node& node)
    {
        mNode = &node;
    }
}