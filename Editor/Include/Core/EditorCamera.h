#pragma once

#include <glm/glm.hpp>

namespace Trinity
{
	class EditorCamera
	{
	public:

		EditorCamera() = default;
		virtual ~EditorCamera() = default;

		EditorCamera(const EditorCamera&) = delete;
		EditorCamera& operator = (const EditorCamera&) = delete;

		EditorCamera(EditorCamera&&) = default;
		EditorCamera& operator = (EditorCamera&&) = default;

		const glm::vec2& getSize() const
		{
			return mSize;
		}

		const glm::vec2& getCenter() const
		{
			return mCenter;
		}

		float getMoveSpeed() const
		{
			return mMoveSpeed;
		}

		float getRotationSpeed() const
		{
			return mRotationSpeed;
		}

		virtual glm::mat4 getView() const;
		virtual glm::mat4 getProjection() const;
		virtual glm::mat4 getViewProj() const;

		virtual void moveRight(float scale);
		virtual void moveUp(float scale);
		virtual void turn(float scale);

		virtual void panTo(const glm::vec2& position);
		virtual void panBy(const glm::vec2& delta);
		virtual void zoomBy(float amount);
		virtual void zoomTo(const glm::vec2& position, float amount);

		virtual void setMoveSpeed(float moveSpeed);
		virtual void setRotationSpeed(float rotationSpeed);
		virtual void setPosition(const glm::vec2& position);
		virtual void setRotation(float rotation);

		virtual void setSize(float left, float right, float top, float bottom);
		virtual void update(float deltaTime);

	protected:

		glm::mat4 mTransform{ 1.0f };
		glm::vec2 mSize{ 0.0f };
		glm::vec2 mCenter{ 0.0f };
		glm::vec2 mPosition{ 0.0f };
		glm::vec2 mPositionDelta{ 0.0f };
		float mRotation{ 0.0f };
		float mRotationDelta{ 0.0f };
		float mMoveSpeed{ 1.0f };
		float mRotationSpeed{ 1.0f };
	};
}