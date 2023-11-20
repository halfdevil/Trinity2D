#include "Scene/ComponentFactory.h"

namespace Trinity
{
	bool ComponentFactory::hasRegister(const UUIDv4::UUID& uuid)
	{
		return mCreators.contains(uuid);
	}

	void ComponentFactory::registerCreator(const UUIDv4::UUID& uuid, ComponentCreator creator)
	{
		mCreators.insert(std::make_pair(uuid, std::move(creator)));
	}

	void ComponentFactory::removeCreator(const UUIDv4::UUID& uuid)
	{
		if (auto it = mCreators.find(uuid); it != mCreators.end())
		{
			mCreators.erase(it);
		}
	}

	std::unique_ptr<Component> ComponentFactory::createComponent(const UUIDv4::UUID& uuid)
	{
		if (auto it = mCreators.find(uuid); it != mCreators.end())
		{
			auto& creator = it->second;
			return creator();
		}

		return nullptr;
	}
}