#pragma once

#include "Physics/RigidShape.h"
#include <array>

namespace Trinity
{
	struct CollisionInfo;

	struct SupportPoint
	{
		glm::vec2 point{ 0.0f };
		float distance{ 0.0f };
	};

	class RectangleShape : public RigidShape
	{
	public:

		RectangleShape();
		virtual ~RectangleShape() = default;

		RectangleShape(const RectangleShape&) = delete;
		RectangleShape& operator = (const RectangleShape&) = delete;

		RectangleShape(RectangleShape&&) = default;
		RectangleShape& operator = (RectangleShape&&) = default;

		const glm::vec2& getOrigin() const
		{
			return mOrigin;
		}

		const glm::vec2& getSize() const
		{
			return mSize;
		}

		const std::array<glm::vec2, 4>& getVertices() const
		{
			return mVertices;
		}

		const std::array<glm::vec2, 4>& getFaceNormals() const
		{
			return mFaceNormals;
		}

		virtual BoundingRect getBoundingRect() const override;

		virtual void setOrigin(const glm::vec2& origin);
		virtual void setSize(const glm::vec2& size);
		virtual void setPosition(const glm::vec2& position) override;
		virtual void setCenter(const glm::vec2& center) override;

		virtual void move(const glm::vec2& value) override;
		virtual void rotate(float value) override;
		virtual void updateInertia() override;

		virtual bool findSupportPoint(const glm::vec2& dir,	const glm::vec2& pointOnEdge, 
			SupportPoint& supportPoint);

		virtual bool findAxisLeastPenetration(RectangleShape& otherRect, 
			CollisionInfo& collisionInfo);

	protected:

		virtual void updateVertices();

	protected:

		glm::vec2 mOrigin{ 0.0f };
		glm::vec2 mSize{ 0.0f };
		std::array<glm::vec2, 4> mVertices;
		std::array<glm::vec2, 4> mFaceNormals;
	};
}