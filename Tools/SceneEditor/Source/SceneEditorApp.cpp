#include "SceneEditorApp.h"
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
#include "Editor/EditorLayout.h"
#include "Widgets/AssetFileDialog.h"
#include "Widgets/MenuBar.h"
#include "Widgets/AssetBrowser.h"
#include "Widgets/SceneHierarchy.h"
#include "Widgets/SceneViewport.h"
#include "Widgets/SceneInspector.h"
#include "Widgets/MessageBox.h"
#include "Widgets/SelectionWindow.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiFont.h"
#include "Scene/Scene.h"
#include "Scene/SceneSystem.h"
#include "Scene/Components/Camera.h"
#include "Scene/ComponentFactory.h"
#include "Graphics/Texture.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/LineCanvas.h"
#include "VFS/FileSystem.h"
#include "ImGuizmo.h"
#include "IconsFontAwesome6.h"
#include <format>

namespace Trinity
{
	bool SceneEditorApp::init()
	{
		if (!EditorApp::init())
		{
			return false;
		}

		mCurrentScene = createDefaultScene();
		if (!mCurrentScene)
		{
			LogError("SceneEditorApp::createDefaultScene() failed");
			return false;
		}

		mViewport = createSceneViewport(mCurrentScene->getName(), *mCurrentScene);
		mInspector = createSceneInspector("Inspector", *mCurrentScene);
		mHierarchy = createSceneHierarchy("Scene", *mCurrentScene);
		mComponentsWindow = createComponentsWindow("Add Component", *mCurrentScene);

		return true;
	}

	void SceneEditorApp::setupInput()
	{
		Application::setupInput();
	}

	void SceneEditorApp::update(float deltaTime)
	{
		if (mViewport != nullptr)
		{
			mViewport->update(deltaTime);
		}
	}

	void SceneEditorApp::onDraw(float deltaTime)
	{
		EditorApp::onDraw(deltaTime);

		if (mViewport != nullptr)
		{
			mViewport->drawContent(deltaTime);
		}
	}

	void SceneEditorApp::onMainMenuClick(const std::string& name)
	{
		EditorApp::onMainMenuClick(name);

		if (name == "openScene")
		{
			if (mFileDialog != nullptr)
			{
				mFileDialog->show(AssetFileDialogType::Open, "Open Scene");
			}
		}
		else if (name == "saveScene")
		{
			if (mFileDialog != nullptr)
			{
				mFileDialog->show(AssetFileDialogType::Save, "Save Scene");
			}
		}
		else if (name == "inspector")
		{
			if (mInspector != nullptr)
			{
				mInspector->setEnabled(true);
			}
		}
	}

	void SceneEditorApp::onAssetFileDialogClick(AssetFileDialogType dialogType, AssetFileDialogResult result, const std::string& path)
	{
		if (result == AssetFileDialogResult::Cancel)
		{
			return;
		}

		if (dialogType == AssetFileDialogType::Open)
		{
			auto* scene = openScene(path);
			if (scene != nullptr)
			{
				if (mCurrentScene != nullptr)
				{
					mResourceCache->removeResource(mCurrentScene);
				}

				mCurrentScene = scene;
				mHierarchy->setScene(*mCurrentScene);
				mViewport->setScene(*mCurrentScene);
				mInspector->setScene(*mCurrentScene);
			}
			else
			{
				mMessageBox->show(std::format("Unable to open scene file '{}'", path), "Error",
					MessageBoxButtons::Ok, MessageBoxIcon::Error);
			}
		}
		else if (dialogType == AssetFileDialogType::Save || dialogType == AssetFileDialogType::SaveAs)
		{
			if (!saveScene(mCurrentScene, path))
			{
				mMessageBox->show(std::format("Unable to save scene file '{}'", path), "Error",
					MessageBoxButtons::Ok, MessageBoxIcon::Error);
			}

			if (mAssetBrowser != nullptr)
			{
				auto& fileSystem = FileSystem::get();
				mAssetBrowser->refreshPath(fileSystem.getDirectory(path));
			}
		}
	}

	Scene* SceneEditorApp::openScene(const std::string& path)
	{
		auto& fileSystem = FileSystem::get();

		auto scene = std::make_unique<Scene>();
		scene->setName(fileSystem.getFileName(path, false));
		scene->setFileName(path);

		SceneSerializer serializer;
		serializer.setScene(*scene);

		auto jsonStr = fileSystem.readText(path);
		if (jsonStr.empty())
		{
			LogError("FileSystem::readText() failed for: '%s'", path.c_str());
			return nullptr;
		}

		json sceneJson = json::parse(jsonStr);
		if (!serializer.read(sceneJson, *mResourceCache))
		{
			LogError("SceneSerializer::read() from json failed for scene with path: '%s'", path.c_str());
			return nullptr;
		}

		auto* scenePtr = scene.get();
		mResourceCache->addResource(std::move(scene));

		return scenePtr;
	}

	bool SceneEditorApp::saveScene(Scene* scene, const std::string& path)
	{
		if (!scene)
		{
			LogError("Scene is null for path: '%s'", path.c_str());
			return false;
		}

		SceneSerializer serializer;
		serializer.setScene(*scene);

		json sceneJson;
		if (!serializer.write(sceneJson))
		{
			LogError("SceneSerializer::write() to json failed for scene with path: '%s'", path.c_str());
			return false;
		}

		auto& fileSystem = FileSystem::get();
		if (!fileSystem.writeText(path, sceneJson.dump(4)))
		{
			LogError("FileSystem::writeText() for scene with path: '%s'", path.c_str());
			return false;
		}

		return true;
	}

	MenuBar* SceneEditorApp::createMainMenu()
	{
		auto* mainMenu = EditorApp::createMainMenu();
		auto* fileMenu = mainMenu->addMenuItem("file", "File");
		auto* editMenu = mainMenu->addMenuItem("edit", "Edit");
		auto* viewMenu = mainMenu->addMenuItem("view", "View");

		mainMenu->addMenuItem("newScene", "  New Scene  ", "CTRL+N", fileMenu);
		mainMenu->addMenuItem("openScene", "  Open Scene  ", "CTRL+O", fileMenu);
		mainMenu->addSeparator(fileMenu);
		mainMenu->addMenuItem("saveScene", "  Save Scene  ", "CTRL+S", fileMenu);
		mainMenu->addMenuItem("saveAsScene", "  Save Scene As  ", "CTRL+SHIFT+S", fileMenu);
		mainMenu->addSeparator(fileMenu);
		mainMenu->addMenuItem("exit", "  Exit  ", "ALT+F4", fileMenu);

		mainMenu->addMenuItem("cut", "  Cut  ", "CTRL+X", editMenu);
		mainMenu->addMenuItem("copy", "  Copy  ", "CTRL+C", editMenu);
		mainMenu->addMenuItem("paste", "  Paste  ", "CTRL+V", editMenu);

		mainMenu->addMenuItem("inspector", "  Inspector  ", "CTRL+I", viewMenu);
		mainMenu->addMenuItem("sceneHierarchy", "  Scene Hierarchy  ", "CTRL+INS+S", viewMenu);
		mainMenu->addMenuItem("sceneViewport", "  Scene Viewport  ", "CTRL+INS+V", viewMenu);

		return mainMenu;
	}

	AssetFileDialog* SceneEditorApp::createFileDialog()
	{
		auto* fileDialog = EditorApp::createFileDialog();
		fileDialog->addFileType("Scene File (*.json)", { ".json" });

		return fileDialog;
	}

	SceneViewport* SceneEditorApp::createSceneViewport(const std::string& title, Scene& scene)
	{
		auto sceneViewport = std::make_unique<SceneViewport>();
		if (!sceneViewport->create(*mResources))
		{
			LogError("Viewport::create() failed");
			return nullptr;
		}

		sceneViewport->setTitle(title);
		sceneViewport->setScene(scene);

		sceneViewport->onResize.subscribe([this](auto width, auto height) {
			onViewportResize(width, height);
		});

		auto* resolution = sceneViewport->getSelectedResolution();
		sceneViewport->resize(resolution->width, resolution->height);

		auto* sceneViewportPtr = sceneViewport.get();
		mWidgets.push_back(std::move(sceneViewport));

		return sceneViewportPtr;
	}

	SceneInspector* SceneEditorApp::createSceneInspector(const std::string& title, Scene& scene)
	{
		auto inspector = std::make_unique<SceneInspector>();
		inspector->setTitle("Inspector");
		inspector->setLayout(*mLayout);
		inspector->setResourceCache(*mResourceCache);
		inspector->setScene(scene);

		auto* inspectorPtr = inspector.get();
		mWidgets.push_back(std::move(inspector));

		return inspectorPtr;
	}

	SceneHierarchy* SceneEditorApp::createSceneHierarchy(const std::string& title, Scene& scene)
	{
		auto sceneHierarchy = std::make_unique<SceneHierarchy>();
		sceneHierarchy->setTitle(title);
		sceneHierarchy->setScene(scene);

		sceneHierarchy->onSelectNode.subscribe([this](auto* selectedNode) {
			onSelectNodeClick(selectedNode);
		});

		auto* sceneHierarchyPtr = sceneHierarchy.get();
		mWidgets.push_back(std::move(sceneHierarchy));

		return sceneHierarchyPtr;
	}

	SelectionWindow* SceneEditorApp::createComponentsWindow(const std::string& title, Scene& scene)
	{
		auto componentNames = scene.getComponentFactory()->getComponentNames();
		std::vector<SelectionItem> items;

		for (auto& name : componentNames)
		{
			items.push_back({ name, name });
		}

		auto componentsWindow = std::make_unique<SelectionWindow>();
		componentsWindow->setTitle(title);
		componentsWindow->setButtonTitle("Add Component");
		componentsWindow->setItems(std::move(items));

		componentsWindow->onSelect.subscribe([this](const auto& item) {
			onComponentSelect(item);
		});

		auto* componentsWindowPtr = componentsWindow.get();
		mWidgets.push_back(std::move(componentsWindow));

		return componentsWindowPtr;
	}

	void SceneEditorApp::onSelectNodeClick(Node* selectedNode)
	{
		mInspector->setSelectedNode(selectedNode);
		mViewport->setSelectedNode(selectedNode);
	}

	void SceneEditorApp::onViewportResize(uint32_t width, uint32_t height)
	{
		if (mViewport != nullptr)
		{
			auto* frameBuffer = mViewport->getFrameBuffer();
			auto* colorTarget = frameBuffer->getColorTexture();

			if (colorTarget != nullptr)
			{
				mImGuiRenderer->invalidateTexture(*colorTarget);
			}
		}
	}

	void SceneEditorApp::onComponentSelect(const SelectionItem& item)
	{
		auto* selectedNode = mHierarchy->getCurrentNode();
		auto& componentName = item.name;

		if (selectedNode != nullptr && !componentName.empty())
		{
			auto* componentFactory = mCurrentScene->getComponentFactory();
			auto component = componentFactory->createComponentByName(componentName);

			if (component != nullptr)
			{
				mCurrentScene->addComponent(std::move(component), *selectedNode);
			}
		}
	}
}

using namespace Trinity;

int main(int argc, char* argv[])
{
	static SceneEditorApp app;
	app.run({
		.logLevel = LogLevel::Info,
		.title = "Trinity2D - Scene Editor",
#ifdef __EMSCRIPTEN__
		.configFile = "/Assets/EditorConfig.json",
#else
		.configFile = "EditorConfig.json",
#endif
	});

	return 0;
}