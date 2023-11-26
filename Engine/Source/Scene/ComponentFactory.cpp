#include "Scene/ComponentFactory.h"

namespace Trinity
{
	void ComponentFactory::registerCreator(const UUIDv4::UUID& uuid, const std::string& name, ComponentCreator creator)
	{
		mCreators.insert(std::make_pair(uuid, std::move(creator)));
		mNameUUIDMap.insert(std::make_pair(name, uuid));
	}

	bool ComponentFactory::hasRegister(const UUIDv4::UUID& uuid)
	{
		return mCreators.contains(uuid);
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

	std::unique_ptr<Component> ComponentFactory::createComponentByName(const std::string& name)
	{
		if (auto it = mNameUUIDMap.find(name); it != mNameUUIDMap.end())
		{
			if (auto it2 = mCreators.find(it->second); it2 != mCreators.end())
			{
				auto& creator = it2->second;
				return creator();
			}
		}

		return nullptr;
	}

	std::vector<std::string> ComponentFactory::getComponentNames() const
	{
		std::vector<std::string> names;
		names.resize(mNameUUIDMap.size());

		std::transform(mNameUUIDMap.begin(), mNameUUIDMap.end(), names.begin(),
			[](const auto& pair) {
				return pair.first;
			}
		);

		return names;
	}
}