#pragma once

#include <random>
#include <memory>
#include <glm/glm.hpp>

namespace Trinity
{
	template <typename T>
	class RandomGenerator
	{
	public:

		RandomGenerator() :
			mGenerator(std::make_unique<T>(std::random_device()())),
			mDistribution(0.0f, 1.0f)
		{
		}

		float getFloat(float min = 0.0f, float max = 1.0f)
		{
			return min + mDistribution(*mGenerator) * (max - min);
		}

		glm::vec2 getVec2(const glm::vec2& min, const glm::vec2& max)
		{
			return glm::vec2{
				min.x + mDistribution(*mGenerator) * (max.x - min.x),
				min.y + mDistribution(*mGenerator) * (max.y - min.y)
			};
		}

		glm::vec4 getVec4(const glm::vec4& min, const glm::vec4& max)
		{
			return glm::vec4{
				min.x + mDistribution(*mGenerator) * (max.x - min.x),
				min.y + mDistribution(*mGenerator) * (max.y - min.y),
				min.z + mDistribution(*mGenerator) * (max.z - min.z),
				min.w + mDistribution(*mGenerator) * (max.w - min.w)
			};
		}

	private:

		std::unique_ptr<T> mGenerator;
		std::uniform_real_distribution<float> mDistribution;
	};
}