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
		virtual UUIDv4::UUID getTypeUUID() const override;

		virtual bool init();
		virtual void update(float deltaTime);
		virtual void resize(uint32_t width, uint32_t height);

	public:

		inline static UUIDv4::UUID UUID = UUIDv4::UUID::fromStrFactory("f79b5991-ab2e-4735-8373-7363d489641c");
	};
}