#pragma once

#include "Core/Singleton.h"
#include "glm/glm.hpp"
#include <string>

namespace Trinity
{
	class DebugHelper : public Singleton<DebugHelper>
	{
	public:

		static constexpr const char* kShader = "/Assets/Engine/Shaders/Debug.wgsl";
		static constexpr uint32_t kCommonBindGroupIndex = 0;

		struct Vertex
		{
			glm::vec3 position{ 0.0f };
			glm::vec4 color{ 0.0f, 0.0f, 0.0f, 1.0f };
		};

		DebugHelper() = default;
		virtual ~DebugHelper() = default;

		DebugHelper(const DebugHelper&) = delete;
		DebugHelper& operator = (const DebugHelper&) = delete;

		DebugHelper(DebugHelper&&) = default;
		DebugHelper& operator = (DebugHelper&&) = default;

		virtual bool create(uint32_t maxLines);
		virtual void destroy();

	protected:

		std::vector<Vertex> mLines; 
	};
}