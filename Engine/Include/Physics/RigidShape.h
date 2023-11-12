#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Trinity
{
	class RigidShape
	{
	public:

	protected:

		glm::vec2 mPosition{ 0.0f };
		glm::vec2 mCenter{ 0.0f };
		glm::vec2 mVelocity{ 0.0f };
		glm::vec2 mAcceleration{ 0.0f };
		float mAngle{ 0.0f };
		float mAngularVelocity{ 0.0f };
		float mAngularAcceleration{ 0.0f };
		float mMass{ 0.0f };
		float mInvMass{ 0.0f };
		float mFriction{ 0.0f };
		float mRestitution{ 0.0f };
		float mBoundingRadius{ 0.0f };
	};
}