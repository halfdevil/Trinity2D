#pragma once

#include "glm/glm.hpp"

namespace Trinity
{
	class Math
	{
	public:

		static bool decomposeTransform(const glm::mat4& transform, glm::vec3& translation, 
			glm::vec3& rotation, glm::vec3& scale);
	};
}