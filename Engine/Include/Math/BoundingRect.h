#pragma once

#include <vector>
#include <glm/glm.hpp>

#define _USE_MATH_DEFINES
#include <cmath>

namespace Trinity
{
	class BoundingRect
	{
	public:

		BoundingRect() = default;
		BoundingRect(const glm::vec2& inMin, const glm::vec2& inMax)
			: min{ inMin }, max{ inMax }
		{
		}

		BoundingRect(const std::vector<glm::vec2>& points);

		glm::vec2 getSize() const;
		glm::vec2 getCenter() const;

		void fromPoints(const std::vector<glm::vec2>& points);
		void combinePoint(const glm::vec2& p);
		void combineRect(const BoundingRect& other);

		bool isPointInside(const glm::vec2& p) const;
		bool isIntersecting(const BoundingRect& other) const;
		bool contains(const BoundingRect& other) const;

	public:

		static BoundingRect combineRects(const std::vector<BoundingRect>& rects);

	public:

		glm::vec2 min{ 0.0f };
		glm::vec2 max{ 0.0f };
	};
}