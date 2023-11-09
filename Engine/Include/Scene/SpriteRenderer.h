#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Trinity
{
	class BatchRenderer;
	class Scene;

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

		virtual bool prepare(Scene& scene);
		virtual void destroy();
		virtual void draw(float deltaTime);

	protected:

		Scene* mScene{ nullptr };
		std::unique_ptr<BatchRenderer> mRenderer{ nullptr };
	};
}