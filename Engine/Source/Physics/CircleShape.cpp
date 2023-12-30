#include "Physics/CircleShape.h"

namespace Trinity
{
	CircleShape::CircleShape() :
		RigidShape(RigidShapeType::Circle)
	{
	}

	BoundingRect CircleShape::getBoundingRect() const
	{
		return {
			{ mCenter.x - mRadius, mCenter.y - mRadius },
			{ mCenter.x + mRadius, mCenter.y + mRadius }
		};
	}

	void CircleShape::init(const glm::vec2& center, float radius)
	{
		mCenter = center;
		mRadius = radius;
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