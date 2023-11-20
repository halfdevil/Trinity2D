#pragma once

#include "Scene/Components/Script.h"
#include <glm/glm.hpp>

namespace Trinity
{
	class Camera;

	class CameraController : public Script
	{
	public:

		CameraController() = default;
		virtual ~CameraController() = default;

		CameraController(const CameraController&) = delete;
		CameraController& operator = (const CameraController&) = delete;

		CameraController(CameraController&&) = delete;
		CameraController& operator = (CameraController&&) = delete;

		Camera* getCamera() const
		{
			return mCamera;
		}

		float getMoveSpeed() const
		{
			return mMoveSpeed;
		}

		float getRotationSpeed() const
		{
			return mRotationSpeed;
		}

		virtual void moveRight(float scale);
		virtual void moveUp(float scale);
		virtual void turn(float scale);

		virtual void panTo(const glm::vec2& position);
		virtual void panBy(const glm::vec2& delta);

		virtual void setMoveSpeed(float moveSpeed);
		virtual void setRotationSpeed(float rotationSpeed);

		virtual bool init() override;
		virtual void update(float deltaTime) override;

		virtual UUIDv4::UUID getUUID() const override;

	public:

		inline static UUIDv4::UUID UUID = UUIDv4::UUID::fromStrFactory("c72f8400-89a0-4072-9a26-fa687f740263");

	protected:

		Camera* mCamera{ nullptr };
		glm::vec3 mTranslation{ 0.0f };
		glm::vec3 mRotation{ 0.0f };
		float mMoveSpeed{ 1.0f };
		float mRotationSpeed{ 1.0f };
	};
}