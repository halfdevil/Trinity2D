#include "Scene/Components/Scripts/CameraController.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/Transform.h"
#include "Scene/Node.h"
#include "glm/gtx/euler_angles.hpp"

namespace Trinity
{
	void CameraController::moveRight(float scale)
	{
		mTranslation.x += scale * mMoveSpeed;
	}

	void CameraController::moveUp(float scale)
	{
		mTranslation.y += scale * mMoveSpeed;
	}

	void CameraController::turn(float scale)
	{
		mRotation.z += scale * mRotationSpeed;
	}

	void CameraController::panTo(const glm::vec2& position)
	{
		auto& transform = getNode()->getTransform();
		auto& translation = transform.getTranslation();
		
		panBy(position - glm::vec2{ translation });
	}

	void CameraController::panBy(const glm::vec2& delta)
	{
		mTranslation = glm::vec3{ delta, 0.0f };
	}

	void CameraController::setMoveSpeed(float moveSpeed)
	{
		mMoveSpeed = moveSpeed;
	}

	void CameraController::setRotationSpeed(float rotationSpeed)
	{
		mRotationSpeed = rotationSpeed;
	}

	bool CameraController::init()
	{
		if (!Script::init())
		{
			return false;
		}

		auto* cameraNode = getNode();
		if (cameraNode->hasComponent<Camera>())
		{
			mCamera = &cameraNode->getComponent<Camera>();
			return true;
		}

		return false;
	}

	void CameraController::update(float deltaTime)
	{
		Script::update(deltaTime);

		mTranslation *= deltaTime;
		mRotation *= deltaTime;

		if (mRotation != glm::vec3{ 0.0f } || mTranslation != glm::vec3{ 0.0f })
		{
			auto& transform = getNode()->getTransform();

			glm::vec3 rotation = transform.getRotation() + mRotation;
			glm::mat4 orientation = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);
			glm::vec3 translation = transform.getTranslation() + glm::vec3(glm::vec4(mTranslation, 1.0f) * orientation);

			transform.setTranslation(translation);
			transform.setRotation(rotation);
		}

		mTranslation = glm::vec3{ 0.0f };
		mRotation = glm::vec3{ 0.0f };
	}

	UUIDv4::UUID CameraController::getTypeUUID() const
	{
		return CameraController::UUID;
	}
}