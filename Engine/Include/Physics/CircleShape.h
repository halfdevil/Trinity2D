#pragma once

#include "Physics/RigidShape.h"

namespace Trinity
{
	class CircleShape : public RigidShape
	{
	public:

		CircleShape();
		virtual ~CircleShape() = default;

		CircleShape(const CircleShape&) = delete;
		CircleShape& operator = (const CircleShape&) = delete;

		CircleShape(CircleShape&&) = default;
		CircleShape& operator = (CircleShape&&) = default;

		float getRadius() const
		{
			return mRadius;
		}

		virtual void init(const glm::vec2& center, float radius);

		virtual BoundingRect getBoundingRect() const override;
		virtual void updateInertia() override;

	protected:

		float mRadius{ 0.0f };
	};
}