#include "Physics/Physics.h"
#include "Physics/RectangleShape.h"
#include "Physics/CircleShape.h"
#include "Math/Math.h"
#include <cmath>

namespace Trinity
{
	bool Physics::collision(RigidShape& shape1, RigidShape& shape2, CollisionInfo& collisionInfo)
	{
		bool result{ false };

		if (shape1.getType() == RigidShapeType::Rectangle)
		{
			if (shape2.getType() == RigidShapeType::Rectangle)
			{
				result = collision((RectangleShape&)shape1, (RectangleShape&)shape2, collisionInfo);
			}
			else
			{
				result = collision((RectangleShape&)shape1, (CircleShape&)shape2, collisionInfo);
			}
		}
		else
		{
			if (shape2.getType() == RigidShapeType::Rectangle)
			{
				result = collision((RectangleShape&)shape2, (CircleShape&)shape2, collisionInfo);
			}
			else
			{
				result = collision((CircleShape&)shape1, (CircleShape&)shape2, collisionInfo);
			}
		}

		return result;
	}

	void Physics::resolve(RigidShape& shape1, RigidShape& shape2, CollisionInfo& collisionInfo)
	{
		auto diff = shape2.getCenter() - shape1.getCenter();
		if (glm::dot(collisionInfo.normal, diff) < 0)
		{
			collisionInfo.changeDir();
		}

		resolveCollision(shape1, shape2, collisionInfo);
	}

	void Physics::setNumRelaxations(bool numRelaxations)
	{
		mNumRelaxations = numRelaxations;
	}

	void Physics::setPosCorrectionFlag(bool posCorrectionFlag)
	{
		mPosCorrectionFlag = posCorrectionFlag;
	}

	void Physics::setPosCorrectionRate(float posCorrectionRate)
	{
		mPosCorrectionRate = posCorrectionRate;
	}

	bool Physics::collision(RectangleShape& rect1, RectangleShape& rect2, CollisionInfo& collisionInfo)
	{
		CollisionInfo collisionInfo1;
		CollisionInfo collisionInfo2;

		bool status1{ false };
		bool status2{ false };

		status1 = rect1.findAxisLeastPenetration(rect2, collisionInfo1);
		if (status1)
		{
			status2 = rect2.findAxisLeastPenetration(rect1, collisionInfo2);
			if (status2)
			{
				if (collisionInfo1.depth < collisionInfo2.depth)
				{
					auto depthVec = collisionInfo1.normal * collisionInfo1.depth;
					collisionInfo.set(collisionInfo1.depth, collisionInfo1.normal, collisionInfo1.start - depthVec);
				}
				else
				{
					collisionInfo.set(collisionInfo2.depth, collisionInfo2.normal * -1.0f, collisionInfo2.start);
				}
			}
		}

		return status1 && status2;
	}

	bool Physics::collision(CircleShape& circle1, CircleShape& circle2, CollisionInfo& collisionInfo)
	{
		auto from1To2 = circle2.getCenter() - circle1.getRadius();
		auto sum = circle1.getRadius() + circle2.getRadius();
		auto dist = glm::length(from1To2);

		if (dist > std::sqrt(sum * sum))
		{
			return false;
		}

		if (dist != 0.0f)
		{
			auto normalFromt2To1 = glm::normalize(from1To2 * -1.0f);
			auto radiusC2 = normalFromt2To1 * circle2.getRadius();

			collisionInfo.set(sum - dist, glm::normalize(from1To2), circle2.getCenter() + radiusC2);
		}
		else
		{
			if (circle1.getRadius() > circle2.getRadius())
			{
				collisionInfo.set(sum, glm::vec2{ 0.0f, -1.0f }, circle1.getCenter() + 
					glm::vec2{ 0.0f, circle1.getRadius() });

				collisionInfo.set(sum, glm::vec2{ 0.0f, -1.0f }, circle2.getCenter() + 
					glm::vec2{ 0.0f, circle2.getRadius() });
			}
		}

		return true;
	}

	bool Physics::collision(RectangleShape& rect, CircleShape& circle, CollisionInfo& collisionInfo)
	{
		bool inside{ true };
		float bestDistance{ -FLT_MAX };

		uint32_t nearestEdge{ 0 };
		glm::vec2 circle2Pos{ 0.0f };
		glm::vec2 projection{ 0.0f };

		auto& vertices = rect.getVertices();
		auto& faceNormals = rect.getFaceNormals();

		for (uint32_t idx = 0; idx < 4; idx++)
		{
			circle2Pos = circle.getCenter();

			auto v = circle2Pos - vertices[idx];
			auto projection = glm::dot(v, faceNormals[idx]);

			if (projection > 0.0f)
			{
				bestDistance = projection;
				nearestEdge = idx;
				inside = false;
				break;
			}

			if (projection > bestDistance)
			{
				bestDistance = projection;
				nearestEdge = idx;
			}
		}

		if (!inside)
		{
			auto v1 = circle2Pos - vertices[nearestEdge];
			auto v2 = vertices[(nearestEdge + 1) % 4] - vertices[nearestEdge];
			auto dt = glm::dot(v1, v2);

			if (dt < 0.0f)
			{
				auto dist = glm::length(v1);
				if (dist > circle.getRadius())
				{
					return false;
				}

				auto normal = glm::normalize(v1);
				auto radiusVec = normal * -circle.getRadius();
				collisionInfo.set(circle.getRadius() - dist, normal, circle2Pos + radiusVec);
			}
			else
			{
				v1 = circle2Pos - vertices[(nearestEdge + 1) % 4];
				v2 = v2 * -1.0f;
				dt = glm::dot(v1, v2);

				if (dt < 0.0f)
				{
					auto dist = glm::length(v1);
					if (dist > circle.getRadius())
					{
						return false;
					}

					auto normal = glm::normalize(v1);
					auto radiusVec = normal * -circle.getRadius();
					collisionInfo.set(circle.getRadius() - dist, normal, circle2Pos + radiusVec);
				}
				else
				{
					if (bestDistance < circle.getRadius())
					{
						auto radiusVec = faceNormals[nearestEdge] * circle.getRadius();
						collisionInfo.set(circle.getRadius() - bestDistance, faceNormals[nearestEdge], 
							circle2Pos + radiusVec);
					}
					else
					{
						return false;
					}
				}
			}
		}
		else
		{
			auto radiusVec = faceNormals[nearestEdge] * circle.getRadius();
			collisionInfo.set(circle.getRadius() - bestDistance, faceNormals[nearestEdge], circle2Pos + radiusVec);
		}

		return true;
	}

	void Physics::positionalCorrection(RigidShape& shape1, RigidShape& shape2, CollisionInfo& collisionInfo)
	{
		auto s1InvMass = shape1.getInverseMass();
		auto s2InvMass = shape2.getInverseMass();

		auto num = collisionInfo.depth / (s1InvMass + s2InvMass) * mPosCorrectionRate;
		auto correctionAmount = collisionInfo.normal * num;

		shape1.move(correctionAmount * -s1InvMass);
		shape2.move(correctionAmount * s2InvMass);
	}

	void Physics::resolveCollision(RigidShape& shape1, RigidShape& shape2, CollisionInfo& collisionInfo)
	{
		if (shape1.getInverseMass() == 0.0f && shape2.getInverseMass() == 0.0f)
		{
			return;
		}

		if (mPosCorrectionFlag)
		{
			positionalCorrection(shape1, shape2, collisionInfo);
		}

		auto start = collisionInfo.start * (shape2.getInverseMass() / (shape1.getInverseMass() + 
			shape2.getInverseMass()));

		auto end = collisionInfo.end * (shape1.getInverseMass() / (shape1.getInverseMass() + 
			shape2.getInverseMass()));

		auto n = collisionInfo.normal;
		auto p = start + end;
		auto r1 = p - shape1.getCenter();
		auto r2 = p - shape2.getCenter();

		auto v1 = shape1.getVelocity() + glm::vec2{ -1.0f * shape1.getAngularVelocity() * r1.y, 
			shape1.getAngularVelocity() * r1.x };

		auto v2 = shape1.getVelocity() + glm::vec2{ -1.0f * shape1.getAngularVelocity() * r2.y,
			shape1.getAngularVelocity() * r2.x };

		auto relVelocity = v2 - v1;
		auto relVelocityNormal = glm::dot(relVelocity, n);

		if (relVelocityNormal > 0.0f)
		{
			return;
		}

		auto newRestitution = std::min(shape1.getRestitution(), shape2.getRestitution());
		auto newFriction = std::min(shape1.getFriction(), shape2.getFriction());
		auto r1CrossN = Math::cross(r1, n);
		auto r2CrossN = Math::cross(r2, n);

		auto jN = -(1.0f + newRestitution) * relVelocityNormal;
		jN = jN / (shape1.getInverseMass() + shape2.getInverseMass() +
			r1CrossN * r1CrossN * shape1.getInertia() +
			r2CrossN * r2CrossN * shape2.getInertia());

		auto impulse = n * jN; 
		shape1.setVelocity(shape1.getVelocity() - impulse * shape1.getInverseMass());
		shape2.setVelocity(shape2.getVelocity() + impulse * shape2.getInverseMass());
		shape1.setAngularVeloctiy(shape1.getAngularVelocity() - r1CrossN * jN * shape1.getInertia());
		shape2.setAngularVeloctiy(shape2.getAngularVelocity() + r2CrossN * jN * shape2.getInertia());

		auto tangent = relVelocity - n * glm::dot(relVelocity, n);
		tangent = glm::normalize(tangent) * -1.0f;

		auto r1CrossT = Math::cross(r1, tangent);
		auto r2CrossT = Math::cross(r2, tangent);

		auto jT = -(1.0f + newRestitution) * glm::dot(relVelocity, tangent) * newFriction;
		jT = jT / (shape1.getInverseMass() + shape2.getInverseMass() +
			r1CrossT * r1CrossT * shape1.getInertia() +
			r2CrossT * r2CrossT * shape2.getInertia());

		if (jT > jN)
		{
			jT = jN;
		}

		impulse = tangent * jT;
		shape1.setVelocity(shape1.getVelocity() - impulse * shape1.getInverseMass());
		shape2.setVelocity(shape2.getVelocity() + impulse * shape2.getInverseMass());
		shape1.setAngularVeloctiy(shape1.getAngularVelocity() - r1CrossT * jT * shape1.getInertia());
		shape2.setAngularVeloctiy(shape2.getAngularVelocity() + r2CrossT * jT * shape2.getInertia());
	}
}