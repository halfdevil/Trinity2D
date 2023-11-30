#pragma once

#include "Core/Singleton.h"
#include "Gui/Widget.h"
#include <functional>
#include <memory>
#include "uuid_v4.h"

namespace Trinity
{
	class WidgetFactory : public Singleton<WidgetFactory>
	{
	public:

		using WidgetCreator = std::function<std::unique_ptr<Widget>()>;

		WidgetFactory() = default;
		virtual ~WidgetFactory() = default;

		WidgetFactory(const WidgetFactory&) = delete;
		WidgetFactory& operator = (const WidgetFactory&) = delete;

		WidgetFactory(WidgetFactory&&) = default;
		WidgetFactory& operator = (WidgetFactory&&) = default;

		virtual void registerCreator(const UUIDv4::UUID& uuid, const std::string& name, WidgetCreator creator);
		virtual bool hasRegister(const UUIDv4::UUID& uuid);
		virtual void removeCreator(const UUIDv4::UUID& uuid);

		virtual std::unique_ptr<Widget> createWidget(const UUIDv4::UUID& uuid);
		virtual std::unique_ptr<Widget> createWidgetByName(const std::string& name);
		virtual std::vector<std::string> getWidgetNames() const;

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

		std::unordered_map<UUIDv4::UUID, WidgetCreator> mCreators;
		std::unordered_map<std::string, UUIDv4::UUID> mNameUUIDMap;
	};
}