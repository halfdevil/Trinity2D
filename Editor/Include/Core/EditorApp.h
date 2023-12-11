#pragma once

#include "Core/Application.h"
#include "Widgets/AssetFileDialog.h"
#include "imgui.h"

namespace Trinity
{
	class ImGuiRenderer;
	class ImGuiFont;
	class EditorTheme;
	class EditorResources;
	class EditorWidget;
	class EditorLayout;
	class EditorGizmo;
	class MenuBar;
	class AssetBrowser;
	class MessageBox;
	class Scene;
	class Camera;

	class EditorApp : public Application
	{
	public:

		EditorApp() = default;
		virtual ~EditorApp() = default;

		EditorApp(const EditorApp&) = delete;
		EditorApp& operator = (const EditorApp&) = delete;

		EditorApp(EditorApp&&) = default;
		EditorApp& operator = (EditorApp&&) = default;

		ImGuiRenderer* getImGuiRenderer() const
		{
			return mImGuiRenderer.get();
		}

		EditorResources* getEditorResource() const
		{
			return mResources.get();
		}

	protected:

		virtual bool init() override;
		virtual void update(float deltaTime) override;
		virtual void draw(float deltaTime) override;
		virtual void setupInput() override;

		virtual void onUpdate(float deltaTime);
		virtual void onDraw(float deltaTime);
		virtual void onPreGui(float deltaTime);
		virtual void onGui();
		virtual void onPostGui();
		
		virtual Scene* createDefaultScene(
			float width = 1024.0f, 
			float height = 768.0f
		);

		virtual MenuBar* createMainMenu();
		virtual AssetFileDialog* createFileDialog();
		virtual MessageBox* createMessageBox();
		virtual AssetBrowser* createAssetBrowser(const std::string& title);

		virtual void onMainMenuClick(const std::string& name);
		virtual void onAssetFileDialogClick(AssetFileDialogType dialogType,	AssetFileDialogResult result,
			const std::string& path);

	protected:

		std::unique_ptr<ImGuiRenderer> mImGuiRenderer{ nullptr };
		std::unique_ptr<RenderPass> mRenderPass{ nullptr };
		std::unique_ptr<EditorTheme> mTheme{ nullptr };
		std::unique_ptr<EditorLayout> mLayout{ nullptr };
		std::unique_ptr<EditorResources> mResources{ nullptr };
		std::vector<std::unique_ptr<EditorWidget>> mWidgets;
		ImGuiFont* mFont{ nullptr };
		MenuBar* mMainMenu{ nullptr };
		AssetBrowser* mAssetBrowser{ nullptr };
		AssetFileDialog* mFileDialog{ nullptr };
		MessageBox* mMessageBox{ nullptr };
	};
}