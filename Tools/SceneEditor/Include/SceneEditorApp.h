#pragma once

#include "Core/EditorApp.h"

namespace Trinity
{
	class SceneHierarchy;
	class SceneViewport;
	class SceneInspector;
	class SelectionWindow;
	class Node;
	struct SelectionItem;

	class SceneEditorApp : public EditorApp
	{
	public:

		SceneEditorApp() = default;
		virtual ~SceneEditorApp() = default;

		SceneEditorApp(const SceneEditorApp&) = delete;
		SceneEditorApp& operator = (const SceneEditorApp&) = delete;

		SceneEditorApp(SceneEditorApp&&) = default;
		SceneEditorApp& operator = (SceneEditorApp&&) = default;

	protected:

		virtual bool init() override;
		virtual void setupInput() override;
		virtual void update(float deltaTime) override;
		virtual void onDraw(float deltaTime) override;

		virtual Scene* openScene(const std::string& path);
		virtual bool saveScene(Scene* scene, const std::string& path);

		virtual MenuBar* createMainMenu() override;
		virtual AssetFileDialog* createFileDialog() override;

		virtual SceneViewport* createSceneViewport(const std::string& title, Scene& scene);
		virtual SceneInspector* createSceneInspector(const std::string& title, Scene& scene);
		virtual SceneHierarchy* createSceneHierarchy(const std::string& title, Scene& scene);
		virtual SelectionWindow* createComponentsWindow(const std::string& title, Scene& scene);

		virtual void onMainMenuClick(const std::string& name) override;
		virtual void onAssetFileDialogClick(AssetFileDialogType dialogType,	AssetFileDialogResult result,
			const std::string& path) override;

		virtual void onSelectNodeClick(Node* selectedNode);
		virtual void onViewportResize(uint32_t width, uint32_t height);
		virtual void onComponentSelect(const SelectionItem& item);

	protected:

		Scene* mCurrentScene{ nullptr };
		SceneHierarchy* mHierarchy{ nullptr };
		SceneViewport* mViewport{ nullptr };
		SceneInspector* mInspector{ nullptr };
		SelectionWindow* mComponentsWindow{ nullptr };
	};
}