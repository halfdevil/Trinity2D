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
		mRotation += scale * mRotationSpeed;
	}

	void CameraController::panTo(const glm::vec2& position)
	{
		auto& transform = getNode()->getTransform();
		auto& translation = transform.getTranslation();
		
		panBy(position - translation);
	}

	void CameraController::panBy(const glm::vec2& delta)
	{
		mTranslation = delta;
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

		if (mRotation != 0.0f || mTranslation != glm::vec2{ 0.0f })
		{
			auto& transform = getNode()->getTransform();

			float rotation = transform.getRotation() + mRotation;
			glm::mat4 orientation = glm::yawPitchRoll(0.0f, 0.0f, rotation);
			glm::vec2 translation = transform.getTranslation() + glm::vec2(glm::vec4(mTranslation, 0.0f, 1.0f) * orientation);

			transform.setTranslation(translation);
			transform.setRotation(rotation);
		}

		mTranslation = glm::vec2{ 0.0f };
		mRotation =  0.0f;
	}

	UUIDv4::UUID CameraController::getTypeUUID() const
	{
		return CameraController::UUID;
	}
}