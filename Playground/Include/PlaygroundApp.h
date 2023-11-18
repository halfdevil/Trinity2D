#pragma once

#include "Core/Application.h"

namespace Trinity
{
	class Font;
	class Scene;
	class Script;
	class Camera;
	class CameraController;
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
		virtual void update(float deltaTime) override;
		virtual void draw(float deltaTime) override;
		virtual void setupInput() override;
		virtual void onGui();

		virtual void moveRight(float scale);
		virtual void moveUp(float scale);
		virtual void turn(float scale);

	protected:

		Font* mFont{ nullptr };
		Scene* mScene{ nullptr };
		Camera* mCamera{ nullptr };
		CameraController* mCameraController{ nullptr };
		Texture* mTexture{ nullptr };
		FrameBuffer* mFrameBuffer{ nullptr };
		std::vector<Script*> mScripts;
		std::unique_ptr<ImGuiRenderer> mImGuiRenderer{ nullptr };
		std::unique_ptr<TextRenderer> mTextRenderer{ nullptr };
		std::unique_ptr<BatchRenderer> mBatchRenderer{ nullptr };
		std::unique_ptr<RenderPass> mRenderPass{ nullptr };
	};
}