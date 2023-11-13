#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "webgpu/webgpu_cpp.h"

namespace Trinity
{
	class BatchRenderer;
	class Scene;
	class RenderTarget;

	class SpriteRenderer
	{
	public:

		static constexpr const char* kShader = "/Assets/Engine/Shaders/SpriteRenderer.wgsl";

		SpriteRenderer() = default;
		virtual ~SpriteRenderer() = default;

		SpriteRenderer(const SpriteRenderer&) = delete;
		SpriteRenderer& operator = (const SpriteRenderer&) = delete;

		SpriteRenderer(SpriteRenderer&&) = default;
		SpriteRenderer& operator = (SpriteRenderer&&) = default;

		virtual bool create(Scene& scene, RenderTarget& renderTarget);
		virtual void destroy();
		virtual void draw(float deltaTime);

	protected:

		Scene* mScene{ nullptr };
		std::unique_ptr<BatchRenderer> mRenderer{ nullptr };
	};
}