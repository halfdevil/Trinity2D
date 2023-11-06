#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <vector>

namespace Trinity
{
	class Component
	{
	public:

		Component() = default;
		virtual ~Component() = default;

		Component(const Component&) = delete;
		Component& operator = (const Component&) = delete;

		Component(Component&&) = default;
		Component& operator = (Component&&) = default;

		const std::string& getName() const
		{
			return mName;
		}

		virtual void setName(const std::string& name);

		virtual std::type_index getType() const = 0;
		virtual std::string getTypeName() const = 0;

	protected:

		std::string mName;
	};
}