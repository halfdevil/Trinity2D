#include "Core/EditorCamera.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"

namespace Trinity
{
	glm::mat4 EditorCamera::getView() const
	{
		return glm::inverse(mTransform);
	}

	glm::mat4 EditorCamera::getProjection() const
	{
		return glm::ortho(
			mCenter.x - mSize.x * 0.5f,
			mCenter.x + mSize.x * 0.5f,
			mCenter.y - mSize.y * 0.5f,
			mCenter.y + mSize.y * 0.5f
		);
	}

	glm::mat4 EditorCamera::getViewProj() const
	{
		return getProjection() * getView();
	}

	void EditorCamera::moveRight(float scale)
	{
		mPositionDelta.x += scale * mMoveSpeed;
	}

	void EditorCamera::moveUp(float scale)
	{
		mPositionDelta.y += scale * mMoveSpeed;
	}

	void EditorCamera::turn(float scale)
	{
		mRotationDelta += scale * mRotationSpeed;
	}

	void EditorCamera::panTo(const glm::vec2& position)
	{
		panBy(position - mPosition);
	}

	void EditorCamera::panBy(const glm::vec2& delta)
	{
		mPositionDelta = delta;
	}

	void EditorCamera::zoomBy(float amount)
	{
		if (amount > 0.0f)
		{
			mSize *= amount;
		}
	}

	void EditorCamera::zoomTo(const glm::vec2& position, float amount)
	{
		auto delta = position - mPosition;
		mPosition = mPosition - (delta * amount);

		zoomBy(amount);
	}

	void EditorCamera::setMoveSpeed(float moveSpeed)
	{
		mMoveSpeed = moveSpeed;
	}

	void EditorCamera::setRotationSpeed(float rotationSpeed)
	{
		mRotationSpeed = rotationSpeed;
	}

	void EditorCamera::setPosition(const glm::vec2& position)
	{
		mPositionDelta = position - mPosition;
	}

	void EditorCamera::setRotation(float rotation)
	{
		mRotationDelta = rotation - mRotation;
	}

	void EditorCamera::setSize(float left, float right, float bottom, float top)
	{
		mSize = glm::vec2{ right - left, top - bottom };
		mCenter = glm::vec2{ left, bottom } + (mSize * 0.5f);
	}

	void EditorCamera::update(float deltaTime)
	{
		if (mRotationDelta != 0.0f || mPositionDelta != glm::vec2{ 0.0f })
		{
			float rotation = mRotation + mRotationDelta;
			glm::mat4 orientation = glm::yawPitchRoll(0.0f, 0.0f, rotation);
			
			mPosition += glm::vec2(glm::vec4(mPositionDelta, 0.0f, 1.0f) * orientation);
			mRotation = rotation;

			mTransform = glm::translate(glm::mat4(1.0f), glm::vec3(mPosition, 0.0f)) *
				orientation;
		}

		mPositionDelta = glm::vec2{ 0.0f };
		mRotation = 0.0f;
	}
}