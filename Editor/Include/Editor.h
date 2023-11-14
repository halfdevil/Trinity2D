#pragma once

#include "Core/Application.h"
#include "IconsFontAwesome6.h"
#include "imgui.h"

namespace Trinity
{
	class ImGuiRenderer;
	class RenderPass;
	class EditorCache;
	class EditorMenu;
	class AssetBrowser;

	class Editor : public Application
	{
	public:

		static constexpr const char* kEditorFont = "/Assets/Fonts/CascadiaCode.ttf";
		static constexpr const char* kEditorIconFont = "/Assets/Fonts/Icons/" FONT_ICON_FILE_NAME_FAR;
		static constexpr ImWchar kIconRanges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };

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
		std::unique_ptr<EditorCache> mEditorCache{ nullptr };
		std::unique_ptr<EditorMenu> mMainMenu{ nullptr };
		std::unique_ptr<AssetBrowser> mAssetBrowser{ nullptr };
	};
}