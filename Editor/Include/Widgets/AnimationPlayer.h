#pragma once

#include "Widgets/Viewport.h"

namespace Trinity
{
	class Scene;
	class Node;
	class SceneSystem;
	class ResourceCache;

	class AnimationPlayer : public Viewport
	{
	public:

		static constexpr float kBottomPanelHeight = 24.0f;
		static constexpr float kAnimationSpeeds[] = { 0.25f, 0.5f, 0.75f, 1.0f, 1.25f, 1.75f, 2.0f };

		AnimationPlayer() = default;
		virtual ~AnimationPlayer() = default;

		AnimationPlayer(const AnimationPlayer&) = delete;
		AnimationPlayer& operator = (const AnimationPlayer&) = delete;

		AnimationPlayer(AnimationPlayer&&) = default;
		AnimationPlayer& operator = (AnimationPlayer&&) = default;

		bool isPlaying() const
		{
			return mPlaying;
		}

		virtual bool create(EditorResources& resources);
		virtual void destroy() override;

		virtual void setScene(Scene& scene);
		virtual void update(float deltaTime) override;
		virtual void drawContent(float deltaTime) override;

	protected:

		virtual void drawBottomPanel(float x, float y, float width, float height) override;
		virtual void onViewportResize(uint32_t width, uint32_t height) override;
		virtual void onPlayPauseClick(bool playing);
		virtual void onStopClick();
		virtual void onSpeedChange(float speed);
		virtual void onLoopClick(bool loop);
		virtual std::string getSpeedStr(float speed);

	protected:
		
		Scene* mScene{ nullptr };
		std::unique_ptr<SceneSystem> mSceneSystem{ nullptr };
		bool mPlaying{ false };
		float mSpeed{ 1.0f };
		bool mLooping{ true };
	};
}