#pragma once

#include "Math/BoundingRect.h"
#include <glm/glm.hpp>

namespace Trinity
{
	enum class RigidShapeType
	{
		Rectangle,
		Circle
	};

	class RigidShape
	{
	public:

		RigidShape(RigidShapeType type);
		virtual ~RigidShape() = default;

		RigidShape(const RigidShape&) = delete;
		RigidShape& operator = (const RigidShape&) = delete;

		RigidShape(RigidShape&&) = default;
		RigidShape& operator = (RigidShape&&) = default;

		RigidShapeType getType() const
		{
			return mType;
		}

		const glm::vec2& getPosition() const
		{
			return mPosition;
		}

		const glm::vec2& getCenter() const
		{
			return mCenter;
		}

		const glm::vec2& getVelocity() const
		{
			return mVelocity;
		}

		const glm::vec2& getAcceleration() const
		{
			return mAcceleration;
		}

		float getAngle() const
		{
			return mAngle;
		}

		float getAngularVelocity() const
		{
			return mAngularVelocity;
		}

		float getAngularAcceleration() const
		{
			return mAngularAcceleration;
		}

		float getInertia() const
		{
			return mInertia;
		}

		float getMass() const
		{
			return mMass;
		}

		float getInverseMass() const
		{
			return mInverseMass;
		}

		float getFriction() const
		{
			return mFriction;
		}

		float getRestitution() const
		{
			return mRestitution;
		}

		virtual BoundingRect getBoundingRect() const = 0;

		virtual void setPosition(const glm::vec2& position);
		virtual void setCenter(const glm::vec2& center);
		virtual void setVelocity(const glm::vec2& velocity);
		virtual void setAcceleration(const glm::vec2& acceleration);
		virtual void setAngularVeloctiy(float angularVelocity);
		virtual void setAngularAcceleration(float angularAcceleration);
		virtual void setInertia(float inertia);
		virtual void setMass(float mass);
		virtual void setFriction(float friction);
		virtual void setRestitution(float restitution);

		virtual void move(const glm::vec2& value);
		virtual void rotate(float value);
		virtual void update(float deltaTime);
		virtual void updateInertia();

	protected:

		RigidShapeType mType{ RigidShapeType::Rectangle };
		glm::vec2 mPosition{ 0.0f };
		glm::vec2 mCenter{ 0.0f };
		glm::vec2 mVelocity{ 0.0f };
		glm::vec2 mAcceleration{ 0.0f };
		float mAngle{ 0.0f };
		float mAngularVelocity{ 0.0f };
		float mAngularAcceleration{ 0.0f };
		float mInertia{ 0.0f };
		float mMass{ 0.0f };
		float mInverseMass{ 0.0f };
		float mFriction{ 0.0f };
		float mRestitution{ 0.0f };
	};
}