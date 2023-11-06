#include "Scene/Components/ScriptContainer.h"
#include "Scene/Components/Script.h"
#include "Scene/Node.h"

namespace Trinity
{
	std::type_index ScriptContainer::getType() const
	{
		return typeid(ScriptContainer);
	}

	std::string ScriptContainer::getTypeName() const
	{
		return getStaticType();
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

	Script& ScriptContainer::getScript(const std::string& type)
	{
		return *mScripts.at(type);
	}

	bool ScriptContainer::hasScript(const std::string& type)
	{
		return mScripts.contains(type);
	}

	void ScriptContainer::setScript(Script& script)
	{
		auto it = mScripts.find(script.getTypeName());
		if (it != mScripts.end())
		{
			it->second = &script;
		}
		else
		{
			mScripts.insert(std::make_pair(script.getTypeName(), &script));
		}
	}

	void ScriptContainer::setNode(Node& node)
	{
		mNode = &node;

		for (auto& it : mScripts)
		{
			it.second->setNode(node);
		}
	}

	std::string ScriptContainer::getStaticType()
	{
		return "ScriptContainer";
	}
}