#include "Scene/Components/Script.h"

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

	bool Script::init()
	{
		return true;
	}

	void Script::update(float deltaTime)
	{
	}

	void Script::resize(uint32_t width, uint32_t height)
	{
	}

	std::string Script::getStaticType()
	{
		return "Script";
	}
}