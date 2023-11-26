#include "Math/Math.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace Trinity
{
	bool Math::decomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
	{
		glm::mat4 local(transform);
		if (glm::epsilonEqual(local[3][3], 0.0f, glm::epsilon<float>()))
		{
			return false;
		}

		for (uint32_t idx = 0; idx < 3; idx++)
		{
			for (uint32_t jdx = 0; jdx < 3; jdx++)
			{
				local[idx][jdx] /= local[3][3];
			}
		}

		if (glm::epsilonNotEqual(local[0][3], 0.0f, glm::epsilon<float>()) ||
			glm::epsilonNotEqual(local[1][3], 0.0f, glm::epsilon<float>()) ||
			glm::epsilonNotEqual(local[2][3], 0.0f, glm::epsilon<float>()))
		{
			local[0][3] = 0.0f;
			local[1][3] = 0.0f;
			local[2][3] = 0.0f;
			local[3][3] = 1.0f;
		}

		translation = glm::vec3{ local[3] };
		local[3] = glm::vec4{ 0.0f, 0.0f, 0.0f, local[3].w };

		glm::vec3 row[3] = {};
		for (uint32_t idx = 0; idx < 3; idx++)
		{
			for (uint32_t jdx = 0; jdx < 3; jdx++)
			{
				row[idx][jdx] = local[idx][jdx];
			}
		}

		scale.x = glm::length(row[0]);
		scale.y = glm::length(row[1]);
		scale.z = glm::length(row[2]);

		row[0] = glm::detail::scale(row[0], 1.0f);
		row[1] = glm::detail::scale(row[1], 1.0f);
		row[2] = glm::detail::scale(row[2], 1.0f);

		rotation.y = asin(-row[0][2]);
		if (cos(rotation.y) != 0.0f)
		{
			rotation.x = atan2(row[1][2], row[2][2]);
			rotation.z = atan2(row[0][1], row[0][0]);
		}
		else
		{
			rotation.x = atan2(-row[2][0], row[1][1]);
			rotation.z = 0;
		}

		return true;
	}

	float Math::cross(const glm::vec2& v1, const glm::vec2& v2)
	{
		return v1.x * v2.y - v1.y * v2.x;
	}
}