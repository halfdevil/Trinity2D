#pragma once

#include "Core/EditorApp.h"

namespace Trinity
{
	class ParticleEffect;
	class ParticleHierarchy;
	class ParticleInspector;
	class ParticleViewport;

	class ParticleEditorApp : public EditorApp
	{
	public:

		ParticleEditorApp() = default;
		virtual ~ParticleEditorApp() = default;

		ParticleEditorApp(const ParticleEditorApp&) = delete;
		ParticleEditorApp& operator = (const ParticleEditorApp&) = delete;

		ParticleEditorApp(ParticleEditorApp&&) = default;
		ParticleEditorApp& operator = (ParticleEditorApp&&) = default;

	protected:

		virtual bool init() override;
		virtual void setupInput() override;

		virtual void onUpdate(float deltaTime) override;
		virtual void onDraw(float deltaTime) override;

		virtual ParticleEffect* openParticle(const std::string& path);
		virtual bool saveParticle(ParticleEffect* sprite, const std::string& path);

		virtual MenuBar* createMainMenu() override;
		virtual AssetFileDialog* createFileDialog() override;

		virtual ParticleEffect* createDefaultParticle() const;
		virtual ParticleHierarchy* createHierarchy(const std::string& title, ParticleEffect& sprite);
		virtual ParticleInspector* createInspector(const std::string& title, ParticleEffect& sprite);
		virtual ParticleViewport* createViewport(const std::string& title, ParticleEffect& sprite);

		virtual void onMainMenuClick(const std::string& name) override;
		virtual void onAssetFileDialogClick(AssetFileDialogType dialogType, AssetFileDialogResult result,
			const std::string& path) override;

		virtual void onSelectEmitterClick(uint32_t selectedEmitter);
		virtual void onViewportResize(uint32_t width, uint32_t height);

	protected:

		ParticleEffect* mCurrentParticle{ nullptr };
		ParticleHierarchy* mHierarchy{ nullptr };
		ParticleInspector* mInspector{ nullptr };
		ParticleViewport* mViewport{ nullptr };
	};
}