#pragma once

#include "Core/EditorApp.h"

namespace Trinity
{
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
		virtual void onDraw(float deltaTime) override;

		virtual MenuBar* createMainMenu() override;
		virtual AssetFileDialog* createFileDialog() override;
	};
}