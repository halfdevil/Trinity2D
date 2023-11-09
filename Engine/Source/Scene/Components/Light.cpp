#include "Scene/Components/Light.h"

namespace Trinity
{
	std::type_index Light::getType() const
	{
		return typeid(Light);
	}

	std::string Light::getTypeName() const
	{
		return getStaticType();
	}

	void Light::setLightType(LightType lightType)
	{
		mLightType = lightType;
	}

	void Light::setLightProperties(const LightProperties& properties)
	{
		mProperties = properties;
	}

	std::string Light::getStaticType()
	{
		return "Light";
	}
}