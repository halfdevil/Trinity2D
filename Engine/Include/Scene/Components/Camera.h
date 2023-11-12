#pragma once

#include "Scene/Component.h"

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
	#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>

namespace Trinity
{
	class Camera : public Component
	{
	public:

		Camera() = default;
		virtual ~Camera() = default;

		Camera(const Camera&) = delete;
		Camera& operator = (const Camera&) = delete;

		Camera(Camera&&) = default;
		Camera& operator = (Camera&&) = default;

		float getLeft() const
		{
			return mLeft;
		}

		float getRight() const
		{
			return mRight;
		}

		float getBottom() const
		{
			return mBottom;
		}

		float getTop() const
		{
			return mTop;
		}

		float getNearPlane() const
		{
			return mNearPlane;
		}

		float getFarPlane() const
		{
			return mFarPlane;
		}

		virtual glm::mat4 getView() const;
		virtual glm::mat4 getProjection() const;

		virtual std::type_index getType() const override;
		virtual std::string getTypeName() const override;

		virtual void setLeft(float left);
		virtual void setRight(float right);
		virtual void setBottom(float bottom);
		virtual void setTop(float top);
		virtual void setNearPlane(float nearPlane);
		virtual void setFarPlane(float farPlane);

	public:

		static std::string getStaticType();

	protected:

		float mLeft{ -1.0f };
		float mRight{ 1.0f };
		float mBottom{ -1.0f };
		float mTop{ 1.0f };
		float mNearPlane{ 0.0f };
		float mFarPlane{ 1.0f };
	};
}