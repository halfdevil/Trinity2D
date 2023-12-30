#pragma once

#include "Scene/Component.h"
#include <glm/glm.hpp>

namespace Trinity
{
	enum class LightType
	{
		Directional = 0,
		Point,
		Spot,
		MAX
	};

	struct LightProperties
	{
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float depth{ 0.0f };
		float intensity{ 1.0f };
		float near{ 0.0f };
		float far{ 0.0f };
	};

	class Light : public Component
	{
	public:

		Light() = default;
		virtual ~Light() = default;

		Light(const Light&) = delete;
		Light& operator = (const Light&) = delete;

		Light(Light&&) = default;
		Light& operator = (Light&&) = default;

		LightType getLightType() const
		{
			return mLightType;
		}

		const LightProperties& getProperties() const
		{
			return mProperties;
		}

		virtual std::type_index getType() const override;
		virtual UUIDv4::UUID getTypeUUID() const override;

		virtual void setLightType(LightType lightType);
		virtual void setLightProperties(const LightProperties& properties);

	public:

		inline static UUIDv4::UUID UUID = UUIDv4::UUID::fromStrFactory("0d535cb3-b16a-4946-94f6-e0ea5187bd44");

	protected:

		LightType mLightType{ LightType::Directional };
		LightProperties mProperties;
	};
}