#pragma once

#include "Core/EditorApp.h"

namespace Trinity
{
	class Gui;
	class Widget;
	class GuiHierarchy;
	class GuiViewport;
	class GuiInspector;
	class SelectionWindow;
	struct SelectionItem;

	class GuiEditorApp : public EditorApp
	{
	public:

		GuiEditorApp() = default;
		virtual ~GuiEditorApp() = default;

		GuiEditorApp(const GuiEditorApp&) = delete;
		GuiEditorApp& operator = (const GuiEditorApp&) = delete;

		GuiEditorApp(GuiEditorApp&&) = default;
		GuiEditorApp& operator = (GuiEditorApp&&) = default;

	protected:

		virtual bool init() override;
		virtual void setupInput() override;
		virtual void onDraw(float deltaTime) override;

		virtual Gui* openGui(const std::string& path);
		virtual bool saveGui(Gui* scene, const std::string& path);

		virtual MenuBar* createMainMenu() override;
		virtual AssetFileDialog* createFileDialog() override;

		virtual Gui* createDefaultGui();
		virtual GuiHierarchy* createGuiHierarchy(const std::string& title, Gui& gui);
		virtual GuiViewport* createGuiViewport(const std::string& title, Gui& gui);
		virtual GuiInspector* createGuiInspector(const std::string& title, Gui& gui);
		virtual SelectionWindow* createWidgetsWindow(const std::string& title, Gui& gui);

		virtual void onMainMenuClick(const std::string& name) override;
		virtual void onAssetFileDialogClick(AssetFileDialogType dialogType, AssetFileDialogResult result,
			const std::string& path) override;

		virtual void onSelectWidgetClick(Widget* selectedWidget);
		virtual void onGuiViewportResize(uint32_t width, uint32_t height);
		virtual void onWidgetSelect(const SelectionItem& item);

	protected:

		Gui* mCurrentGui{ nullptr };
		GuiHierarchy* mGuiHierarchy{ nullptr };
		GuiViewport* mGuiViewport{ nullptr };
		GuiInspector* mGuiInspector{ nullptr };
		SelectionWindow* mWidgetsWindow{ nullptr };
	};
}