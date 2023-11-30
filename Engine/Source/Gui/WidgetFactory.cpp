#include "Gui/WidgetFactory.h"

namespace Trinity
{
	void WidgetFactory::registerCreator(const UUIDv4::UUID& uuid, const std::string& name, WidgetCreator creator)
	{
		mCreators.insert(std::make_pair(uuid, std::move(creator)));
		mNameUUIDMap.insert(std::make_pair(name, uuid));
	}

	bool WidgetFactory::hasRegister(const UUIDv4::UUID& uuid)
	{
		return mCreators.contains(uuid);
	}

	void WidgetFactory::removeCreator(const UUIDv4::UUID& uuid)
	{
		if (auto it = mCreators.find(uuid); it != mCreators.end())
		{
			mCreators.erase(it);
		}
	}

	std::unique_ptr<Widget> WidgetFactory::createWidget(const UUIDv4::UUID& uuid)
	{
		if (auto it = mCreators.find(uuid); it != mCreators.end())
		{
			auto& creator = it->second;
			return creator();
		}

		return nullptr;
	}

	std::unique_ptr<Widget> WidgetFactory::createWidgetByName(const std::string& name)
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

	std::vector<std::string> WidgetFactory::getWidgetNames() const
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