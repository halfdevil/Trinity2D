#include "Physics/RectangleShape.h"

namespace Trinity
{
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
	}

	void RectangleShape::setPosition(const glm::vec2& position)
	{
		mPosition = position;
		mCenter = { position.x + mSize.x * mOrigin.x, position.y + mSize.y * mOrigin.y };

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