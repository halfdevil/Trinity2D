#include "Physics/RectangleShape.h"
#include "Physics/Physics.h"

namespace Trinity
{
	RectangleShape::RectangleShape()
		: RigidShape(RigidShapeType::Rectangle)
	{
	}

	BoundingRect RectangleShape::getBoundingRect() const
	{
		return { { mVertices.begin(), mVertices.end() } };
	}

	void RectangleShape::setOrigin(const glm::vec2& origin)
	{
		mOrigin = origin;
	}

	void RectangleShape::setSize(const glm::vec2& size)
	{
		mSize = size;
		mCenter = {
			mPosition.x + mSize.x * mOrigin.x,
			mPosition.y + mSize.y * mOrigin.y
		};

		updateVertices();
	}

	void RectangleShape::setPosition(const glm::vec2& position)
	{
		mPosition = position;
		mCenter = { 
			mPosition.x + mSize.x * mOrigin.x, 
			mPosition.y + mSize.y * mOrigin.y 
		};

		updateVertices();
	}

	void RectangleShape::setCenter(const glm::vec2& center)
	{
		mCenter = center;
		updateVertices();
	}

	void RectangleShape::move(const glm::vec2& value)
	{
		RigidShape::move(value);

		for (auto& vertex : mVertices)
		{
			vertex += value;
		}
	}

	void RectangleShape::rotate(float value)
	{
		RigidShape::rotate(value);
	}

	void RectangleShape::updateInertia()
	{
		if (mInverseMass == 0.0f)
		{
			mInertia = 0.0f;
		}
		else
		{
			mInertia = mMass * (mSize.x * mSize.x  + mSize.y * mSize.y) / 12.0f;
		}
	}

	bool RectangleShape::findSupportPoint(const glm::vec2& dir, const glm::vec2& pointOnEdge, SupportPoint& supportPoint)
	{
		bool found = false;
		supportPoint.distance = -FLT_MAX;

		for (uint32_t idx = 0; idx < 4; idx++)
		{
			auto toEdge = mVertices[idx] - pointOnEdge;
			auto projection = glm::dot(toEdge, dir);

			if (projection > 0.0f && projection > supportPoint.distance)
			{
				found = true;
				supportPoint.distance = projection;
				supportPoint.point = mVertices[idx];
			}
		}

		return found;
	}

	bool RectangleShape::findAxisLeastPenetration(RectangleShape& otherRect, CollisionInfo& collisionInfo)
	{
		float bestDistance{ FLT_MAX };
		glm::vec2 point{ 0.0f };

		bool hasSupport{ false };
		int32_t index = 0;
		int32_t bestIndex = -1;

		while (hasSupport && index < (int32_t)mFaceNormals.size())
		{
			auto n = mFaceNormals[index];
			auto dir = n * -1.0f;
			auto& pointOnEdge = mVertices[index];

			SupportPoint supportPoint;
			hasSupport = otherRect.findSupportPoint(dir, pointOnEdge, supportPoint);

			if (hasSupport && supportPoint.distance < bestDistance)
			{
				bestIndex = index;
				bestDistance = supportPoint.distance;
				point = supportPoint.point;
			}

			index++;
		}

		if (hasSupport)
		{
			auto bestVertex = mFaceNormals[bestIndex] * bestDistance;
			collisionInfo.set(bestDistance, mFaceNormals[bestIndex], point + bestVertex);
		}

		return hasSupport;
	}

	void RectangleShape::updateVertices()
	{
		mVertices[0] = { mCenter.x - mSize.x / 2, mCenter.y - mSize.y / 2 };
		mVertices[1] = { mCenter.x + mSize.x / 2, mCenter.y - mSize.y / 2 };
		mVertices[2] = { mCenter.x + mSize.x / 2, mCenter.y + mSize.y / 2 };
		mVertices[3] = { mCenter.x - mSize.x / 2, mCenter.y + mSize.y / 2 };

		mFaceNormals[0] = glm::normalize(mVertices[1] - mVertices[0]);
		mFaceNormals[1] = glm::normalize(mVertices[2] - mVertices[3]);
		mFaceNormals[2] = glm::normalize(mVertices[3] - mVertices[0]);
		mFaceNormals[3] = glm::normalize(mVertices[0] - mVertices[1]);
	}
}