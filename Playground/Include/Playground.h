#pragma once

#include "Core/Application.h"

namespace Trinity
{
	class Font;
	class Scene;
	class Camera;
	class Texture;
	class ImGuiRenderer;
	class TextRenderer;
	class BatchRenderer;

	class Playground : public Application
	{
	public:

		Playground() = default;
		virtual ~Playground() = default;

		Playground(const Playground&) = delete;
		Playground& operator = (const Playground&) = delete;

		Playground(Playground&&) = default;
		Playground& operator = (Playground&&) = default;

	protected:

		virtual bool init() override;
		virtual void draw(float deltaTime) override;
		virtual void setupInput() override;
		virtual void onGui();

	protected:

		Font* mFont{ nullptr };
		Scene* mScene{ nullptr };
		Camera* mCamera{ nullptr };
		Texture* mTexture{ nullptr };
		std::unique_ptr<ImGuiRenderer> mImGuiRenderer{ nullptr };
		std::unique_ptr<TextRenderer> mTextRenderer{ nullptr };
		std::unique_ptr<BatchRenderer> mBatchRenderer{ nullptr };
	};
}