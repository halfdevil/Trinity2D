#pragma once

#include "Core/EditorApp.h"

namespace Trinity
{
	class Sprite;
	class SpriteHierarchy;
	class SpriteInspector;
	class SpriteViewport;
	class SpriteAnimationPlayer;
	struct SpriteFrame;
	struct SpriteAnimation;

	class SpriteEditorApp : public EditorApp
	{
	public:

		SpriteEditorApp() = default;
		virtual ~SpriteEditorApp() = default;

		SpriteEditorApp(const SpriteEditorApp&) = delete;
		SpriteEditorApp& operator = (const SpriteEditorApp&) = delete;

		SpriteEditorApp(SpriteEditorApp&&) = default;
		SpriteEditorApp& operator = (SpriteEditorApp&&) = default;

	protected:

		virtual bool init() override;
		virtual void setupInput() override;

		virtual void onUpdate(float deltaTime) override;
		virtual void onDraw(float deltaTime) override;

		virtual Sprite* openSprite(const std::string& path);
		virtual bool saveSprite(Sprite* sprite, const std::string& path);

		virtual MenuBar* createMainMenu() override;
		virtual AssetFileDialog* createFileDialog() override;

		virtual Sprite* createDefaultSprite() const;
		virtual SpriteHierarchy* createSpriteHierarchy(const std::string& title, Sprite& sprite);
		virtual SpriteInspector* createSpriteInspector(const std::string& title, Sprite& sprite);
		virtual SpriteViewport* createSpriteViewport(const std::string& title, Sprite& sprite);
		virtual SpriteAnimationPlayer* createSpriteAnimationPlayer(const std::string& title, Sprite& sprite);

		virtual void onMainMenuClick(const std::string& name) override;
		virtual void onAssetFileDialogClick(AssetFileDialogType dialogType, AssetFileDialogResult result,
			const std::string& path) override;

		virtual void onSelectFrameHierarchyClick(uint32_t selectedFrame);
		virtual void onSelectFrameViewportClick(uint32_t selectedFrame);
		virtual void onSelectAnimationClick(uint32_t selectedAnimation);
		virtual void onViewportResize(uint32_t width, uint32_t height);
		virtual void onPlayerResize(uint32_t width, uint32_t height);

	protected:

		Sprite* mCurrentSprite{ nullptr };
		SpriteHierarchy* mHierarchy{ nullptr };
		SpriteInspector* mInspector{ nullptr };
		SpriteViewport* mViewport{ nullptr };
		SpriteAnimationPlayer* mAnimPlayer{ nullptr };
	};
}