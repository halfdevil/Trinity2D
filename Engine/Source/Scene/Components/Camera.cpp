#include "Scene/Components/Camera.h"
#include "Scene/Node.h"

namespace Trinity
{
	std::type_index Camera::getType() const
	{
		return typeid(Camera);
	}

	std::string Camera::getTypeName() const
	{
		return getStaticType();
	}

	void Camera::setProjection(const glm::mat4& projection)
	{
		mProjection = projection;
	}

	void Camera::setView(const glm::mat4& view)
	{
		mView = view;
	}

	void Camera::setNode(Node& node)
	{
		mNode = &node;
	}

	std::string Camera::getStaticType()
	{
		return "Camera";
	}
}