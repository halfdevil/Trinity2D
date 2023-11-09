#pragma once

#include "Scene/Component.h"

namespace Trinity
{
	class Script : public Component
	{
	public:

		Script() = default;
		virtual ~Script() = default;

		Script(const Script&) = delete;
		Script& operator = (const Script&) = delete;

		Script(Script&&) = default;
		Script& operator = (Script&&) = default;

		virtual std::type_index getType() const override;
		virtual std::string getTypeName() const override;

		virtual bool init();
		virtual void update(float deltaTime);
		virtual void resize(uint32_t width, uint32_t height);

	public:

		static std::string getStaticType();
	};
}