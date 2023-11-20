#pragma once

#include "Scene/Component.h"
#include <unordered_map>

namespace Trinity
{
	class Script;

	class ScriptContainer : public Component
	{
	public:

		ScriptContainer() = default;
		virtual ~ScriptContainer() = default;

		ScriptContainer(const ScriptContainer&) = delete;
		ScriptContainer& operator = (const ScriptContainer&) = delete;

		ScriptContainer(ScriptContainer&&) = default;
		ScriptContainer& operator = (ScriptContainer&&) = default;

		virtual std::type_index getType() const override;
		virtual UUIDv4::UUID getUUID() const override;

		virtual void init();
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
}