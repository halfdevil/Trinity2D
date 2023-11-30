#include "Scene/Components/ScriptContainer.h"
#include "Scene/Components/Script.h"
#include "Editor/EditorLayout.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/Logger.h"

namespace Trinity
{
	IEditor* ScriptContainer::getEditor(Scene& scene)
	{
		static ScriptContainerEditor editor;
		editor.setScene(scene);
		editor.setScriptContainer(*this);

		return &editor;
	}

	ISerializer* ScriptContainer::getSerializer(Scene& scene)
	{
		static ScriptContainerSerializer serializer;
		serializer.setScriptContainer(*this);
		serializer.setScene(scene);

		return &serializer;
	}

	std::type_index ScriptContainer::getType() const
	{
		return typeid(ScriptContainer);
	}

	UUIDv4::UUID ScriptContainer::getTypeUUID() const
	{
		return ScriptContainer::UUID;
	}

	bool ScriptContainer::init()
	{
		for (auto& it : mScripts)
		{
			if (!it.second->init())
			{
				return false;
			}
		}

		return true;
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
		auto it = mScripts.find(script.getTypeUUID());
		if (it != mScripts.end())
		{
			it->second = &script;
		}
		else
		{
			mScripts.insert(std::make_pair(script.getTypeUUID(), &script));
		}
	}

	void ScriptContainerEditor::setScriptContainer(ScriptContainer& scriptContainer)
	{
		mScriptContainer = &scriptContainer;
		setComponent(scriptContainer);
	}

	void ScriptContainerEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{		
	}

	void ScriptContainerSerializer::setScriptContainer(ScriptContainer& scriptContainer)
	{
		mScriptContainer = &scriptContainer;
		setComponent(scriptContainer);
	}

	bool ScriptContainerSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		if (!ComponentSerializer::read(reader, cache))
		{
			LogError("ComponentSerialzer::read() failed");
			return false;
		}

		return true;
	}

	bool ScriptContainerSerializer::write(FileWriter& writer)
	{
		if (!ComponentSerializer::write(writer))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		return true;
	}

	bool ScriptContainerSerializer::read(json& object, ResourceCache& cache)
	{
		if (!ComponentSerializer::read(object, cache))
		{
			LogError("ComponentSerialzer::read() failed");
			return false;
		}

		return true;
	}

	bool ScriptContainerSerializer::write(json& object)
	{
		if (!ComponentSerializer::write(object))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		return true;
	}
}