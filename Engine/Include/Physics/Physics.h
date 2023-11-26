#pragma once

#include "Core/Singleton.h"
#include "glm/glm.hpp"

namespace Trinity
{
	class RigidShape;
	class RectangleShape;
	class CircleShape;

	struct CollisionInfo
	{
		float depth{ 0.0f };
		glm::vec2 normal{ 0.0f };
		glm::vec2 start{ 0.0f };
		glm::vec2 end{ 0.0f };

		virtual void set(float inDepth, const glm::vec2& inNormal, const glm::vec2& inStart)
		{
			depth = inDepth;
			normal = inNormal;
			start = inStart;

			end = normal * depth;
			end += start;
		}

		virtual void changeDir()
		{
			std::swap(start, end);
			normal *= -1;
		}
	};

	class Physics : public Singleton<Physics>
	{
	public:

		Physics() = default;
		virtual ~Physics() = default;

		Physics(const Physics&) = delete;
		Physics& operator = (const Physics&) = delete;

		Physics(Physics&&) = default;
		Physics& operator = (Physics&&) = default;

		uint32_t getNumRelaxations() const
		{
			return mNumRelaxations;
		}

		bool getPosCorrectionFlag() const
		{
			return mPosCorrectionFlag;
		}

		float getPosCorrectionRate() const
		{
			return mPosCorrectionRate;
		}

		virtual bool collision(RigidShape& shape1, RigidShape& shape2, CollisionInfo& collisionInfo);
		virtual void resolve(RigidShape& shape1, RigidShape& shape2, CollisionInfo& collisionInfo);

		virtual void setNumRelaxations(bool numRelaxations);
		virtual void setPosCorrectionFlag(bool posCorrectionFlag);
		virtual void setPosCorrectionRate(float posCorrectionRate);

	protected:

		virtual bool collision(RectangleShape& rect1, RectangleShape& rect2, CollisionInfo& collisionInfo);
		virtual bool collision(CircleShape& circle1, CircleShape& circle2, CollisionInfo& collisionInfo);
		virtual bool collision(RectangleShape& rect, CircleShape& circle, CollisionInfo& collisionInfo);

		virtual void positionalCorrection(RigidShape& shape1, RigidShape& shape2, CollisionInfo& collisionInfo);
		virtual void resolveCollision(RigidShape& shape1, RigidShape& shape2, CollisionInfo& collisionInfo);

	protected:

		uint32_t mNumRelaxations{ 15 };
		bool mPosCorrectionFlag{ true };
		float mPosCorrectionRate{ 1.0f };
	};
}