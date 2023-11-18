#include "Scene/Components/Camera.h"
#include "Scene/Node.h"

namespace Trinity
{
	glm::mat4 Camera::getView() const
	{
		auto& transform = mNode->getTransform();
		return glm::inverse(transform.getWorldMatrix());
	}

	glm::mat4 Camera::getProjection() const
	{
		float halfWidth = 0.5f * mSize.x;
		float halfHeight = 0.5f * mSize.y;

		return glm::ortho(-halfWidth, halfWidth, halfHeight, -halfHeight, mNearPlane, mFarPlane);
	}

	std::type_index Camera::getType() const
	{
		return typeid(Camera);
	}

	std::string Camera::getTypeName() const
	{
		return getStaticType();
	}

	void Camera::setSize(const glm::vec2& size)
	{
		mSize = size;
	}

	void Camera::setNearPlane(float nearPlane)
	{
		mNearPlane = nearPlane;
	}

	void Camera::setFarPlane(float farPlane)
	{
		mFarPlane = farPlane;
	}

	std::string Camera::getStaticType()
	{
		return "Camera";
	}
}