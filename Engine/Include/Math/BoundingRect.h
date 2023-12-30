#pragma once

#include <vector>
#include <glm/glm.hpp>

#define _USE_MATH_DEFINES
#include <cmath>

namespace Trinity
{
	enum BoundCollideStatus : uint32_t
	{
		Left	= (1 << 0),
		Right	= (1 << 1),
		Top		= (1 << 2),
		Bottom	= (1 << 3),
		Inside	= (1 << 4),
		Outside = 0
	};

	class BoundingRect
	{
	public:

		BoundingRect() = default;
		BoundingRect(const glm::vec2& inMin, const glm::vec2& inMax)
			: min{ inMin }, max{ inMax }
		{
		}

		BoundingRect(const std::vector<glm::vec2>& points);

		virtual glm::vec2 getSize() const;
		virtual glm::vec2 getCenter() const;

		virtual void transform(const glm::mat4& m);
		virtual BoundingRect getTransformed(const glm::mat4& m) const;

		virtual void fromPoints(const std::vector<glm::vec2>& points);
		virtual void combinePoint(const glm::vec2& p);
		virtual void combineRect(const BoundingRect& other);

		virtual bool isPointInside(const glm::vec2& p) const;
		virtual bool isIntersecting(const BoundingRect& other) const;
		virtual bool contains(const BoundingRect& other) const;

		virtual BoundCollideStatus collideStatus(const BoundingRect& other) const;

	public:

		static BoundingRect combineRects(const std::vector<BoundingRect>& rects);

	public:

		glm::vec2 min{ 0.0f };
		glm::vec2 max{ 0.0f };
	};
}