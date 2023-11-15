#include "Physics/RigidShape.h"

namespace Trinity
{
	void RigidShape::setPosition(const glm::vec2& position)
	{
		mPosition = position;
	}

	void RigidShape::setCenter(const glm::vec2& center)
	{
		mCenter = center;
	}

	void RigidShape::setVelocity(const glm::vec2& velocity)
	{
		mVelocity = velocity;
	}

	void RigidShape::setAcceleration(const glm::vec2& acceleration)
	{
		mAcceleration = acceleration;
	}

	void RigidShape::setAngularVeloctiy(float angularVelocity)
	{
		mAngularVelocity = angularVelocity;
	}

	void RigidShape::setAngularAcceleration(float angularAcceleration)
	{
		mAngularAcceleration = angularAcceleration;
	}

	void RigidShape::setInertia(float inertia)
	{
		mInertia = inertia;
	}

	void RigidShape::setMass(float mass)
	{
		if (mass != 0.0f)
		{
			mInverseMass = 1.0f / mass;
		}
		else
		{
			mInverseMass = 0.0f;
			mAcceleration = glm::vec2{ 0.0f };
		}

		mMass = mass;
	}

	void RigidShape::setFriction(float friction)
	{
		mFriction = friction;
	}

	void RigidShape::setRestitution(float restitution)
	{
		mRestitution = restitution;
	}

	void RigidShape::move(const glm::vec2& value)
	{
		if (value.x == 0.0f && value.y == 0.0f)
		{
			return;
		}

		mCenter += value;
		mPosition += value;
	}

	void RigidShape::rotate(float value)
	{
		if (value == 0.0f)
		{
			return;
		}

		mAngle += value;
	}

	void RigidShape::update(float deltaTime)
	{
		mVelocity += mAcceleration * deltaTime;
		mAngularVelocity += mAngularAcceleration * deltaTime;

		move(mVelocity * deltaTime);
		rotate(mAngularVelocity * deltaTime);
	}

	void RigidShape::updateInertia()
	{
	}
}