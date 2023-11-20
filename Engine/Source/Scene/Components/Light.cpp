#include "Scene/Components/Light.h"

namespace Trinity
{
	std::type_index Light::getType() const
	{
		return typeid(Light);
	}

	UUIDv4::UUID Light::getUUID() const
	{
		return Light::UUID;
	}

	void Light::setLightType(LightType lightType)
	{
		mLightType = lightType;
	}

	void Light::setLightProperties(const LightProperties& properties)
	{
		mProperties = properties;
	}
}