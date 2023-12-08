#include "GuiEditorApp.h"
#include "Core/EditorLayout.h"
#include "Core/EditorTheme.h"
#include "Core/EditorResources.h"
#include "Core/EditorGizmo.h"
#include "Core/EditorCamera.h"
#include "Core/EditorGrid.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "Core/Debugger.h"
#include "Core/Clock.h"
#include "Core/Window.h"
#include "Gui/Gui.h"
#include "Gui/GuiSystem.h"
#include "Gui/WidgetFactory.h"
#include "Gui/Widget.h"
#include "Gui/Widgets/Panel.h"
#include "Scene/Components/Camera.h"
#include "Editor/EditorLayout.h"
#include "Widgets/AssetFileDialog.h"
#include "Widgets/MenuBar.h"
#include "Widgets/AssetBrowser.h"
#include "Widgets/MessageBox.h"
#include "Widgets/GuiHierarchy.h"
#include "Widgets/GuiViewport.h"
#include "Widgets/GuiInspector.h"
#include "Widgets/SelectionWindow.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiFont.h"
#include "Graphics/Texture.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/LineCanvas.h"
#include "VFS/FileSystem.h"
#include <format>

namespace Trinity
{
	bool GuiEditorApp::init()
	{
		if (!EditorApp::init())
		{
			return false;
		}

		mCurrentGui = createDefaultGui();
		if (!mCurrentGui)
		{
			LogError("GuiEditorApp::createDefaultGui() failed");
			return false;
		}

		mGuiViewport = createGuiViewport(mCurrentGui->getName(), *mCurrentGui);
		mGuiHierarchy = createGuiHierarchy("Gui", *mCurrentGui);
		mGuiInspector = createGuiInspector("Inspector", *mCurrentGui);
		mWidgetsWindow = createWidgetsWindow("Widgets", *mCurrentGui);

		return true;
	}

	void GuiEditorApp::setupInput()
	{
		Application::setupInput();
	}

	void GuiEditorApp::update(float deltaTime)
	{
		if (mGuiViewport != nullptr)
		{
			mGuiViewport->update(deltaTime);
		}
	}

	void GuiEditorApp::onDraw(float deltaTime)
	{
		EditorApp::onDraw(deltaTime);

		if (mGuiViewport != nullptr)
		{
			mGuiViewport->drawContent(deltaTime);
		}
	}

	Gui* GuiEditorApp::openGui(const std::string& path)
	{
		auto& fileSystem = FileSystem::get();

		auto gui = std::make_unique<Gui>();
		gui->setName(fileSystem.getFileName(path, false));
		gui->setFileName(path);

		auto jsonStr = fileSystem.readText(path);
		if (jsonStr.empty())
		{
			LogError("FileSystem::readText() failed for: '%s'", path.c_str());
			return nullptr;
		}

		json guiJson = json::parse(jsonStr);
		if (!gui->getSerializer()->read(guiJson, *mResourceCache))
		{
			LogError("GuiSerializer::read() from json failed for gui with path: '%s'", path.c_str());
			return nullptr;
		}

		auto* guiPtr = gui.get();
		mResourceCache->addResource(std::move(gui));

		return guiPtr;
	}

	bool GuiEditorApp::saveGui(Gui* gui, const std::string& path)
	{
		if (!gui)
		{
			LogError("Gui is null for path: '%s'", path.c_str());
			return false;
		}

		json guiJson;
		if (!gui->getSerializer()->write(guiJson))
		{
			LogError("GuiSerializer::write() to json failed for gui with path: '%s'", path.c_str());
			return false;
		}

		auto& fileSystem = FileSystem::get();
		if (!fileSystem.writeText(path, guiJson.dump(4)))
		{
			LogError("FileSystem::writeText() for gui with path: '%s'", path.c_str());
			return false;
		}

		return true;
	}

	MenuBar* GuiEditorApp::createMainMenu()
	{
		auto* mainMenu = EditorApp::createMainMenu();
		auto* fileMenu = mainMenu->addMenuItem("file", "File");
		auto* editMenu = mainMenu->addMenuItem("edit", "Edit");
		auto* viewMenu = mainMenu->addMenuItem("view", "View");

		mainMenu->addMenuItem("newGui", "  New Gui  ", "CTRL+N", fileMenu);
		mainMenu->addMenuItem("openGui", "  Open Gui  ", "CTRL+O", fileMenu);
		mainMenu->addSeparator(fileMenu);
		mainMenu->addMenuItem("saveGui", "  Save Gui  ", "CTRL+S", fileMenu);
		mainMenu->addMenuItem("saveAsGui", "  Save Gui As  ", "CTRL+SHIFT+S", fileMenu);
		mainMenu->addSeparator(fileMenu);
		mainMenu->addMenuItem("exit", "  Exit  ", "ALT+F4", fileMenu);

		mainMenu->addMenuItem("cut", "  Cut  ", "CTRL+X", editMenu);
		mainMenu->addMenuItem("copy", "  Copy  ", "CTRL+C", editMenu);
		mainMenu->addMenuItem("paste", "  Paste  ", "CTRL+V", editMenu);
		mainMenu->addMenuItem("inspector", "  Inspector  ", "CTRL+I", viewMenu);

		return mainMenu;
	}

	AssetFileDialog* GuiEditorApp::createFileDialog()
	{
		auto* fileDialog = EditorApp::createFileDialog();
		fileDialog->addFileType("Gui File (*.json)", { ".json" });

		return fileDialog;
	}

	Gui* GuiEditorApp::createDefaultGui() const
	{
		auto gui = std::make_unique<Gui>();
		gui->setName("New Gui");

		auto root = std::make_unique<Panel>();
		root->setSize(glm::vec2{ 500, 500 });
		
		root->setName("Root");
		gui->setRoot(*root);
		gui->addWidget(std::move(root));

		auto* guiPtr = gui.get();
		mResourceCache->addResource(std::move(gui));

		return guiPtr;
	}

	GuiHierarchy* GuiEditorApp::createGuiHierarchy(const std::string& title, Gui& gui)
	{
		auto guiHierarchy = std::make_unique<GuiHierarchy>();
		guiHierarchy->setTitle(title);
		guiHierarchy->setGui(gui);

		guiHierarchy->onSelectWidget.subscribe([this](auto* currentWidget) {
			onSelectWidgetClick(currentWidget);
		});

		auto* guiHierarchyPtr = guiHierarchy.get();
		mWidgets.push_back(std::move(guiHierarchy));

		return guiHierarchyPtr;
	}

	GuiViewport* GuiEditorApp::createGuiViewport(const std::string& title, Gui& gui)
	{
		auto guiViewport = std::make_unique<GuiViewport>();
		if (!guiViewport->create(*mResources))
		{
			LogError("Viewport::create() failed");
			return nullptr;
		}

		guiViewport->setTitle(title);
		guiViewport->setGui(gui);

		guiViewport->onResize.subscribe([this](auto width, auto height) {
			onGuiViewportResize(width, height);
		});

		auto* resolution = guiViewport->getSelectedResolution();
		guiViewport->resize(resolution->width, resolution->height);

		auto* guiViewportPtr = guiViewport.get();
		mWidgets.push_back(std::move(guiViewport));

		return guiViewportPtr;
	}

	GuiInspector* GuiEditorApp::createGuiInspector(const std::string& title, Gui& gui)
	{
		auto inspector = std::make_unique<GuiInspector>();
		inspector->setTitle("Inspector");
		inspector->setLayout(*mLayout);
		inspector->setResourceCache(*mResourceCache);
		inspector->setGui(gui);

		auto* inspectorPtr = inspector.get();
		mWidgets.push_back(std::move(inspector));

		return inspectorPtr;
	}

	SelectionWindow* GuiEditorApp::createWidgetsWindow(const std::string& title, Gui& gui)
	{
		auto widgetNames = gui.getWidgetFactory()->getWidgetNames();
		std::vector<SelectionItem> items;

		for (auto& name : widgetNames)
		{
			items.push_back({ name, name });
		}

		auto widgetsWindow = std::make_unique<SelectionWindow>();
		widgetsWindow->setTitle(title);
		widgetsWindow->setButtonTitle("Add Widget");
		widgetsWindow->setItems(std::move(items));

		widgetsWindow->onSelect.subscribe([this](const auto& item) {
			onWidgetSelect(item);
		});

		auto* widgetsWindowPtr = widgetsWindow.get();
		mWidgets.push_back(std::move(widgetsWindow));

		return widgetsWindowPtr;
	}

	void GuiEditorApp::onMainMenuClick(const std::string& name)
	{

	}

	void GuiEditorApp::onAssetFileDialogClick(AssetFileDialogType dialogType, 
		AssetFileDialogResult result, const std::string& path)
	{

	}

	void GuiEditorApp::onSelectWidgetClick(Widget* selectedWidget)
	{
		mGuiViewport->setSelectedWidget(selectedWidget);
		mGuiInspector->setSelectedWidget(selectedWidget);
	}

	void GuiEditorApp::onGuiViewportResize(uint32_t width, uint32_t height)
	{
		if (mGuiViewport != nullptr)
		{
			auto* frameBuffer = mGuiViewport->getFrameBuffer();
			auto* colorTarget = frameBuffer->getColorTexture();

			if (colorTarget != nullptr)
			{
				mImGuiRenderer->invalidateTexture(*colorTarget);
			}
		}

		if (mCurrentGui != nullptr)
		{
			mCurrentGui->getRoot()->setSize(glm::vec2{ (float)width, (float)height });
			mCurrentGui->layoutWidgets();
		}
	}

	void GuiEditorApp::onWidgetSelect(const SelectionItem& item)
	{
		auto* selectedWidget = mGuiHierarchy->getCurrentWidget();
		auto& widgetName = item.name;

		if (selectedWidget != nullptr && !widgetName.empty())
		{
			auto* widgetFactory = mCurrentGui->getWidgetFactory();
			auto widget = widgetFactory->createWidgetByName(widgetName);
			widget->setName("Panel");

			if (widget != nullptr)
			{
				selectedWidget->addChild(*widget);
				mCurrentGui->addWidget(std::move(widget));
			}
		}
	}
}

using namespace Trinity;

int main(int argc, char* argv[])
{
	static GuiEditorApp app;
	app.run({
		.logLevel = LogLevel::Info,
		.title = "Trinity2D - Gui Editor",
#ifdef __EMSCRIPTEN__
		.configFile = "/Assets/EditorConfig.json",
#else
		.configFile = "EditorConfig.json",
#endif
	});

	return 0;
}