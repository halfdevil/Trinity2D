#pragma once

#include "Scene/Component.h"
#include "Editor/Editor.h"
#include "VFS/Serializer.h"
#include <unordered_map>

namespace Trinity
{
	class Script;
	class ScriptContainerEditor;
	class ScriptContainerSerializer;

	class ScriptContainer : public Component
	{
	public:

		friend class ScriptContainerEditor;
		friend class ScriptContainerSerializer;

		ScriptContainer() = default;
		virtual ~ScriptContainer() = default;

		ScriptContainer(const ScriptContainer&) = delete;
		ScriptContainer& operator = (const ScriptContainer&) = delete;

		ScriptContainer(ScriptContainer&&) = default;
		ScriptContainer& operator = (ScriptContainer&&) = default;

		virtual IEditor* getEditor(Scene& scene) override;
		virtual ISerializer* getSerializer(Scene& scene) override;

		virtual std::type_index getType() const override;
		virtual UUIDv4::UUID getTypeUUID() const override;

		virtual bool init();
		virtual void update(float deltaTime);
		virtual void resize(uint32_t width, uint32_t height);

		virtual Script& getScript(const UUIDv4::UUID& uuid);
		virtual bool hasScript(const UUIDv4::UUID& uuid);
		virtual void setScript(Script& script);

	public:

		inline static UUIDv4::UUID UUID = UUIDv4::UUID::fromStrFactory("c5e2d6f0-4524-4463-9b26-a0f61d9dbc6c");

	public:

		template <typename T>
		inline T& getScript()
		{
			return dynamic_cast<T&>(getScript(T::UUID));
		}

		template <typename T>
		inline bool hasScript()
		{
			return hasScript(T::UUID);
		}

	protected:

		std::unordered_map<UUIDv4::UUID, Script*> mScripts;
	};

	class ScriptContainerEditor : public ComponentEditor
	{
	public:

		ScriptContainerEditor() = default;
		virtual ~ScriptContainerEditor() = default;

		virtual void setScriptContainer(ScriptContainer& scriptContainer);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		ScriptContainer* mScriptContainer{ nullptr };
	};

	class ScriptContainerSerializer : public ComponentSerializer
	{
	public:

		ScriptContainerSerializer() = default;
		virtual ~ScriptContainerSerializer() = default;

		ScriptContainerSerializer(const ScriptContainerSerializer&) = delete;
		ScriptContainerSerializer& operator = (const ScriptContainerSerializer&) = delete;

		ScriptContainerSerializer(ScriptContainerSerializer&&) = default;
		ScriptContainerSerializer& operator = (ScriptContainerSerializer&&) = default;

		virtual void setScriptContainer(ScriptContainer& scriptContainer);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		ScriptContainer* mScriptContainer{ nullptr };
	};
}