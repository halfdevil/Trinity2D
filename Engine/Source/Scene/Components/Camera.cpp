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
		return glm::ortho(mLeft, mRight, mBottom, mTop, mNearPlane, mFarPlane);
	}

	std::type_index Camera::getType() const
	{
		return typeid(Camera);
	}

	std::string Camera::getTypeName() const
	{
		return getStaticType();
	}

	void Camera::setLeft(float left)
	{
		mLeft = left;
	}

	void Camera::setRight(float right)
	{
		mRight = right;
	}

	void Camera::setBottom(float bottom)
	{
		mBottom = bottom;
	}

	void Camera::setTop(float top)
	{
		mTop = top;
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