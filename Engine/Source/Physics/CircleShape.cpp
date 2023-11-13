#include "Physics/CircleShape.h"

namespace Trinity
{
	BoundingRect CircleShape::getBoundingRect() const
	{
		return {
			{ mCenter.x - mRadius, mCenter.y - mRadius },
			{ mCenter.x + mRadius, mCenter.y + mRadius }
		};
	}

	void CircleShape::setRadius(float radius)
	{
		mRadius = radius;
	}

	void CircleShape::setPosition(const glm::vec2& position)
	{
		mPosition = position;
		mCenter = { mPosition.x + mRadius, mPosition.y + mRadius };
	}

	void CircleShape::updateInertia()
	{
		if (mInverseMass == 0.0f)
		{
			mInertia = 0.0f;
		}
		else
		{
			mInertia = mMass * (mRadius * mRadius) / 12.0f;
		}
	}
}