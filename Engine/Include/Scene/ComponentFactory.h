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

		ComponentCreator& getCreator(const UUIDv4::UUID& uuid);

		bool hasRegister(const UUIDv4::UUID& uuid);
		void registerCreator(const UUIDv4::UUID& uuid, ComponentCreator creator);
		void removeCreator(const UUIDv4::UUID& uuid);

		std::unique_ptr<Component> createComponent(const UUIDv4::UUID& uuid);

	public:

		template <typename T>
		inline void registerCreator()
		{
			registerCreator(T::UUID, []() {
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
			return dynamic_cast<T&>(getCreator(T::UUID));
		}

	protected:

		std::unordered_map<UUIDv4::UUID, ComponentCreator> mCreators;
	};
}