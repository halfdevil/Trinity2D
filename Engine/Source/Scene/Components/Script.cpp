#include "Scene/Components/Script.h"

namespace Trinity
{
	std::type_index Script::getType() const
	{
		return typeid(Script);
	}

	UUIDv4::UUID Script::getTypeUUID() const
	{
		return Script::UUID;
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
}