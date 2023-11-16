#pragma once

#include "Core/Singleton.h"
#include "Core/Application.h"
#include "IconsFontAwesome6.h"
#include "imgui.h"

namespace Trinity
{
	class ImGuiRenderer;
	class ImGuiFont;
	class RenderPass;
	class EditorResources;
	class EditorWidget;

	class Editor : public Application, public Singleton<Editor>
	{
	public:

		static constexpr ImWchar kIconFontRanges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
		static constexpr float kIconFontSizes[] = { 64.0f };

		Editor() = default;
		virtual ~Editor() = default;

		Editor(const Editor&) = delete;
		Editor& operator = (const Editor&) = delete;

		Editor(Editor&&) = default;
		Editor& operator = (Editor&&) = default;

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
		virtual bool loadFonts();

		virtual void onGui();
		virtual void onMainMenuClick(const std::string& title);

	protected:

		std::unique_ptr<ImGuiRenderer> mImGuiRenderer{ nullptr };
		std::unique_ptr<RenderPass> mRenderPass{ nullptr };
		std::unique_ptr<EditorResources> mEditorResources{ nullptr };
		std::vector<std::unique_ptr<EditorWidget>> mWidgets;
		ImGuiFont* mFont{ nullptr };
	};
}