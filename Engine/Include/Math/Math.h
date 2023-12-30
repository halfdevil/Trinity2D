#pragma once

#include "glm/glm.hpp"

namespace Trinity
{
	class Math
	{
	public:

		static bool decomposeTransform(const glm::mat4& transform, glm::vec3& translation, 
			glm::vec3& rotation, glm::vec3& scale);

		static float cross(const glm::vec2& v1, const glm::vec2& v2);
		static glm::vec2 rotate(const glm::vec2& v, float angle, const glm::vec2& ref);
	};
}