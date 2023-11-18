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
	class FrameBuffer;
	class RenderPass;

	class PlaygroundApp : public Application
	{
	public:

		PlaygroundApp() = default;
		virtual ~PlaygroundApp() = default;

		PlaygroundApp(const PlaygroundApp&) = delete;
		PlaygroundApp& operator = (const PlaygroundApp&) = delete;

		PlaygroundApp(PlaygroundApp&&) = default;
		PlaygroundApp& operator = (PlaygroundApp&&) = default;

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
		Texture* mColorTexture{ nullptr };
		Texture* mDepthTexture{ nullptr };
		FrameBuffer* mFrameBuffer{ nullptr };
		std::unique_ptr<ImGuiRenderer> mImGuiRenderer{ nullptr };
		std::unique_ptr<TextRenderer> mTextRenderer{ nullptr };
		std::unique_ptr<BatchRenderer> mBatchRenderer{ nullptr };
		std::unique_ptr<RenderPass> mRenderPass{ nullptr };
	};
}