#pragma once

#include "Core/Singleton.h"
#include "Scene/Component.h"
#include <functional>
#include <memory>
#include "uuid_v4.h"

namespace Trinity
{
	class ComponentFactory : public Singleton<ComponentFactory>
	{
	public:

		using ComponentCreator = std::function<std::unique_ptr<Component>()>;

		void registerCreator(const UUIDv4::UUID& uuid, const std::string& name, ComponentCreator creator);
		bool hasRegister(const UUIDv4::UUID& uuid);
		void removeCreator(const UUIDv4::UUID& uuid);

		std::unique_ptr<Component> createComponent(const UUIDv4::UUID& uuid);
		std::unique_ptr<Component> createComponentByName(const std::string& name);
		std::vector<std::string> getComponentNames() const;

	public:

		template <typename T>
		inline void registerCreator(const std::string& name)
		{
			registerCreator(T::UUID, name, []() {
				return std::make_unique<T>();
			});
		}

		template <typename T>
		inline void hasRegister()
		{
			return hasRegister(T::UUID);
		}

		template <typename T>
		inline T& getCreator()
		{
			return (T&)getCreator(T::UUID);
		}

	protected:

		std::unordered_map<UUIDv4::UUID, ComponentCreator> mCreators;
		std::unordered_map<std::string, UUIDv4::UUID> mNameUUIDMap;
	};
}