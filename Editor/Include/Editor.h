#pragma once

#include "Core/Application.h"

namespace Trinity
{
	class ImGuiRenderer;
	class RenderPass;
	class EditorMenu;

	class Editor : public Application
	{
	public:

		Editor() = default;
		virtual ~Editor() = default;

		Editor(const Editor&) = delete;
		Editor& operator = (const Editor&) = delete;

		Editor(Editor&&) = default;
		Editor& operator = (Editor&&) = default;

	protected:

		virtual bool init() override;
		virtual void draw(float deltaTime) override;
		virtual void setupInput() override;
		virtual void onGui();
		virtual void onMainMenuClick(const std::string& title);

	protected:

		std::unique_ptr<ImGuiRenderer> mImGuiRenderer{ nullptr };
		std::unique_ptr<RenderPass> mRenderPass{ nullptr };
		std::unique_ptr<EditorMenu> mMainMenu{ nullptr };
	};
}