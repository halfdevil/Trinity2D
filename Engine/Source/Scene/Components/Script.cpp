#include "Scene/Components/Script.h"
#include "Scene/Node.h"

namespace Trinity
{
	std::type_index Script::getType() const
	{
		return typeid(Script);
	}

	std::string Script::getTypeName() const
	{
		return getStaticType();
	}

	void Script::init()
	{
	}

	void Script::update(float deltaTime)
	{
	}

	void Script::resize(uint32_t width, uint32_t height)
	{
	}

	void Script::setNode(Node& node)
	{
		mNode = &node;
	}

	std::string Script::getStaticType()
	{
		return "Script";
	}
}