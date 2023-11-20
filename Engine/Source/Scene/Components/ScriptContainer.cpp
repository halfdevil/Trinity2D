#include "Scene/Components/ScriptContainer.h"
#include "Scene/Components/Script.h"

namespace Trinity
{
	std::type_index ScriptContainer::getType() const
	{
		return typeid(ScriptContainer);
	}

	UUIDv4::UUID ScriptContainer::getUUID() const
	{
		return ScriptContainer::UUID;
	}

	void ScriptContainer::init()
	{
		for (auto& it : mScripts)
		{
			it.second->init();
		}
	}

	void ScriptContainer::update(float deltaTime)
	{
		for (auto& it : mScripts)
		{
			it.second->update(deltaTime);
		}
	}

	void ScriptContainer::resize(uint32_t width, uint32_t height)
	{
		for (auto& it : mScripts)
		{
			it.second->resize(width, height);
		}
	}

	Script& ScriptContainer::getScript(const UUIDv4::UUID& uuid)
	{
		return *mScripts.at(uuid);
	}

	bool ScriptContainer::hasScript(const UUIDv4::UUID& uuid)
	{
		return mScripts.contains(uuid);
	}

	void ScriptContainer::setScript(Script& script)
	{
		auto it = mScripts.find(script.getUUID());
		if (it != mScripts.end())
		{
			it->second = &script;
		}
		else
		{
			mScripts.insert(std::make_pair(script.getUUID(), &script));
		}
	}
}