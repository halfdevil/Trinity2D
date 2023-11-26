#pragma once

#include "Core/Application.h"
#include "Widgets/AssetFileDialog.h"
#include "imgui.h"

namespace Trinity
{
	class ImGuiRenderer;
	class ImGuiFont;
	class PhysicsSystem;
	class EditorTheme;
	class EditorResources;
	class EditorWidget;
	class EditorLayout;
	class EditorGizmo;
	class MenuBar;
	class AssetBrowser;
	class MessageBox;
	class SceneHierarchy;
	class SceneViewport;
	class Inspector;
	class Scene;
	class Node;
	class Camera;
	class Texture;

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
			return mEditorResources.get();
		}

	protected:

		virtual bool init() override;
		virtual void draw(float deltaTime) override;
		virtual void setupInput() override;
		virtual void onGui();
		
		virtual Scene* createDefaultScene(
			float width = 1024.0f, 
			float height = 768.0f,
			float nearPlane = 0.0f, 
			float farPlane = 100.0f
		);

		virtual Scene* openScene(const std::string& path);
		virtual bool saveScene(Scene* scene, const std::string& path);

		virtual MenuBar* createMainMenu();
		virtual AssetFileDialog* createFileDialog();
		virtual MessageBox* createMessageBox();
		virtual AssetBrowser* createAssetBrowser(const std::string& title);
		virtual SceneViewport* createSceneViewport(const std::string& title, Scene& scene);
		virtual Inspector* createInspector(const std::string& title, Scene& scene);
		virtual SceneHierarchy* createSceneHierarchy(const std::string& title, Scene& scene);

		virtual void onMainMenuClick(const std::string& name);
		virtual void onSelectNodeClick(Node* selectedNode);
		virtual void onAssetFileDialogClick(
			AssetFileDialogType dialogType,
			AssetFileDialogResult result,
			const std::string& path);

	protected:

		std::unique_ptr<ImGuiRenderer> mImGuiRenderer{ nullptr };
		std::unique_ptr<RenderPass> mRenderPass{ nullptr };
		std::unique_ptr<EditorTheme> mTheme{ nullptr };
		std::unique_ptr<EditorLayout> mLayout{ nullptr };
		std::unique_ptr<EditorResources> mEditorResources{ nullptr };
		std::vector<std::unique_ptr<EditorWidget>> mWidgets;
		ResourceCache* mResourceCache{ nullptr };
		ImGuiFont* mFont{ nullptr };
		Scene* mEditorScene{ nullptr };
		Scene* mCurrentScene{ nullptr };
		Camera* mEditorCamera{ nullptr };
		MenuBar* mMainMenu{ nullptr };
		AssetBrowser* mAssetBrowser{ nullptr };
		SceneHierarchy* mSceneHierarchy{ nullptr };
		SceneViewport* mSceneViewport{ nullptr };
		Inspector* mInspector{ nullptr };
		AssetFileDialog* mFileDialog{ nullptr };
		MessageBox* mMessageBox{ nullptr };
	};
}