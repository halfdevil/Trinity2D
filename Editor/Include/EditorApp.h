#pragma once

#include "Core/Application.h"
#include "imgui.h"

namespace Trinity
{
	class ImGuiRenderer;
	class ImGuiFont;
	class RenderPass;
	class EditorResources;
	class EditorWidget;
	class SceneHierarchy;
	class Inspector;
	class Scene;

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
		virtual void onMainMenuClick(const std::string& title);

	protected:

		std::unique_ptr<ImGuiRenderer> mImGuiRenderer{ nullptr };
		std::unique_ptr<RenderPass> mRenderPass{ nullptr };
		std::unique_ptr<EditorResources> mEditorResources{ nullptr };
		std::vector<std::unique_ptr<EditorWidget>> mWidgets;
		ImGuiFont* mFont{ nullptr };
		Scene* mTestScene{ nullptr };
		SceneHierarchy* mSceneHierarchy{ nullptr };
		Inspector* mInspector{ nullptr };
	};
}