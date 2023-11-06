#include "Scene/ComponentFactory.h"

namespace Trinity
{
	void ComponentFactory::registerCreator(const std::string& type, ComponentCreator creator)
	{
		mCreators.insert(std::make_pair(type, std::move(creator)));
	}

	void ComponentFactory::removeCreator(const std::string& type)
	{
		if (auto it = mCreators.find(type); it != mCreators.end())
		{
			mCreators.erase(it);
		}
	}

	std::unique_ptr<Component> ComponentFactory::createComponent(const std::string& type)
	{
		if (auto it = mCreators.find(type); it != mCreators.end())
		{
			auto& creator = it->second;
			return creator();
		}

		return nullptr;
	}
}