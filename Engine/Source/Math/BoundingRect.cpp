#include "Math/BoundingRect.h"

namespace Trinity
{
	BoundingRect::BoundingRect(const std::vector<glm::vec2>& points)
	{
		fromPoints(points);
	}

	glm::vec2 BoundingRect::getSize() const
	{
		return glm::vec2(max[0] - min[0], max[1] - min[1]);
	}

	glm::vec2 BoundingRect::getCenter() const
	{
		return 0.5f * glm::vec2(max[0] + min[0], max[1] + min[1]);
	}

	void BoundingRect::transform(const glm::mat4& m)
	{
		std::vector<glm::vec2> corners =
		{
			{ min.x, min.y },
			{ min.x, max.y },
			{ max.x, min.y },
			{ max.x, max.y }
		};

		for (auto& v : corners)
		{
			v = glm::vec2(m * glm::vec4(v, 0.0f, 1.0f));
		}

		fromPoints(corners);
	}

	BoundingRect BoundingRect::getTransformed(const glm::mat4& m) const
	{
		BoundingRect b = *this;
		b.transform(m);

		return b;
	}

	void BoundingRect::fromPoints(const std::vector<glm::vec2>& points)
	{
		glm::vec2 vmin(FLT_MAX);
		glm::vec2 vmax(-FLT_MAX);

		for (uint32_t i = 0; i < (uint32_t)points.size(); i++)
		{
			if (points[i].x > vmax.x) vmax.x = points[i].x;
			if (points[i].y > vmax.y) vmax.y = points[i].y;

			if (points[i].x < vmin.x) vmin.x = points[i].x;
			if (points[i].y < vmin.y) vmin.y = points[i].y;
		}

		min = vmin;
		max = vmax;
	}

	void BoundingRect::combinePoint(const glm::vec2& p)
	{
		min = glm::min(min, p);
		max = glm::max(max, p);
	}

	void BoundingRect::combineRect(const BoundingRect& other)
	{
		combinePoint(other.min);
		combinePoint(other.max);
	}

	bool BoundingRect::isPointInside(const glm::vec2& p) const
	{
		return p.x >= min.x && p.x <= max.x &&
			p.y >= min.y && p.y <= max.y;
	}

	bool BoundingRect::isIntersecting(const BoundingRect& other) const
	{
		return min.x <= other.max.x && max.x >= other.min.x &&
			min.y <= other.max.y && max.y >= other.min.y;
	}

	bool BoundingRect::contains(const BoundingRect& other) const
	{
		return min.x <= other.min.x && max.x >= other.max.x &&
			min.y <= other.min.y && max.y >= other.max.y;
	}

	BoundingRect BoundingRect::combineRects(const std::vector<BoundingRect>& boxes)
	{
		std::vector<glm::vec2> allPoints;
		allPoints.reserve(boxes.size() * 4);

		for (const auto& box : boxes)
		{
			allPoints.emplace_back(box.min.x, box.min.y);
			allPoints.emplace_back(box.min.x, box.max.y);
			allPoints.emplace_back(box.max.x, box.min.y);
			allPoints.emplace_back(box.max.x, box.max.y);
		}

		return { allPoints };
	}
}