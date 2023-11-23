#pragma once

#include "Core/Application.h"
#include "Widgets/AssetFileDialog.h"
#include "imgui.h"

namespace Trinity
{
	class ImGuiRenderer;
	class ImGuiFont;
	class TextureRenderer;
	class RenderPass;
	class EditorResources;
	class EditorWidget;
	class EditorGizmo;
	class AssetBrowser;
	class MessageBox;
	class SceneHierarchy;
	class Inspector;
	class Viewport;
	class Scene;
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
		virtual void onMainMenuClick(const std::string& name);
		virtual void onAssetFileDialogClick(AssetFileDialogType dialogType, 
			AssetFileDialogResult result, const std::string& path);
		
		virtual Scene* createDefaultScene(
			float width = 1024.0f, 
			float height = 768.0f,
			float nearPlane = 0.1f, 
			float farPlane = 100.0f
		);

		virtual Scene* openScene(const std::string& path);
		virtual bool saveScene(Scene* scene, const std::string& path);

	protected:

		std::unique_ptr<ImGuiRenderer> mImGuiRenderer{ nullptr };
		std::unique_ptr<TextureRenderer> mTextureRenderer{ nullptr };
		std::unique_ptr<RenderPass> mRenderPass{ nullptr };
		std::unique_ptr<EditorResources> mEditorResources{ nullptr };
		std::vector<std::unique_ptr<EditorWidget>> mWidgets;
		ResourceCache* mResourceCache{ nullptr };
		ImGuiFont* mFont{ nullptr };
		Scene* mEditorScene{ nullptr };
		Scene* mCurrentScene{ nullptr };
		Camera* mEditorCamera{ nullptr };
		AssetBrowser* mAssetBrowser{ nullptr };
		SceneHierarchy* mSceneHierarchy{ nullptr };
		Viewport* mSceneViewport{ nullptr };
		Inspector* mInspector{ nullptr };
		AssetFileDialog* mFileDialog{ nullptr };
		MessageBox* mMessageBox{ nullptr };
	};
}