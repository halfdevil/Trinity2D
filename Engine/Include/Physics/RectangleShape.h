#pragma once

#include "Physics/RigidShape.h"
#include <array>

namespace Trinity
{
	class RectangleShape : public RigidShape
	{
	public:

		RectangleShape() = default;
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

		virtual BoundingRect getBoundingRect() const override;

		virtual void setOrigin(const glm::vec2& origin);
		virtual void setSize(const glm::vec2& size);
		virtual void setPosition(const glm::vec2& position) override;
		virtual void setCenter(const glm::vec2& center) override;

		virtual void move(const glm::vec2& value) override;
		virtual void rotate(float value) override;
		virtual void updateInertia() override;

	protected:

		virtual void updateVertices();

	protected:

		glm::vec2 mOrigin{ 0.0f };
		glm::vec2 mSize{ 0.0f };
		std::array<glm::vec2, 4> mVertices;
		std::array<glm::vec2, 4> mFaceNormals;
	};
}