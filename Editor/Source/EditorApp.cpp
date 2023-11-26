#include "EditorApp.h"
#include "Editor/EditorLayout.h"
#include "Widgets/AssetFileDialog.h"
#include "Widgets/MenuBar.h"
#include "Widgets/AssetBrowser.h"
#include "Widgets/SceneHierarchy.h"
#include "Widgets/SceneViewport.h"
#include "Widgets/Inspector.h"
#include "Widgets/MessageBox.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiFont.h"
#include "Scene/Scene.h"
#include "Scene/RenderSystem.h"
#include "Scene/Components/Camera.h"
#include "Scene/ComponentFactory.h"
#include "Graphics/Texture.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/BatchRenderer.h"
#include "VFS/FileSystem.h"
#include "Core/EditorLayout.h"
#include "Core/EditorTheme.h"
#include "Core/EditorResources.h"
#include "Core/EditorGizmo.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "Core/Debugger.h"
#include "Core/Clock.h"
#include "Core/Window.h"
#include "ImGuizmo.h"
#include "IconsFontAwesome6.h"
#include <format>

namespace Trinity
{
	bool EditorApp::init()
	{
		if (!Application::init())
		{
			return false;
		}

		auto& swapChain = mGraphicsDevice->getSwapChain();

		mGraphicsDevice->setClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });
		mRenderPass = std::make_unique<RenderPass>();

		mImGuiRenderer = std::make_unique<ImGuiRenderer>();
		if (!mImGuiRenderer->create(*mWindow, swapChain))
		{
			LogError("ImGuiRenderer::create() failed!!");
			return false;
		}

		mEditorResources = std::make_unique<EditorResources>();
		if (!mEditorResources->create(mConfig["resources"]))
		{
			LogError("EditorCache::create() failed");
			return false;
		}

		mTheme = std::make_unique<EditorTheme>();
		if (!mTheme->create("/Assets/Editor/Themes/DefaultTheme.json"))
		{
			LogError("EditorTheme::create() failed");
			return false;
		}

		mResourceCache = mEditorResources->getResourceCache();
		mLayout = std::make_unique<EditorLayout>();

		if (!mLayout->updateFiles("/Assets", FileType::Texture))
		{
			LogError("EditorLayout::updateFiles() failed for 'texture'");
			return false;
		}

		mEditorScene = createDefaultScene();
		if (!mEditorScene)
		{
			LogError("EditorApp::createDefaultScene() for editor failed");
			return false;
		}

		mCurrentScene = createDefaultScene();
		if (!mCurrentScene)
		{
			LogError("EditorApp::createDefaultScene() failed");
			return false;
		}

		auto cameras = mEditorScene->getComponents<Camera>();
		mEditorCamera = cameras[0];

		mMainMenu = createMainMenu();
		mFileDialog = createFileDialog();
		mMessageBox = createMessageBox();
		mAssetBrowser = createAssetBrowser(ICON_FA_GLOBE " Asset Browser");
		mSceneViewport = createSceneViewport(mCurrentScene->getName(), *mCurrentScene);
		mInspector = createInspector("Inspector", *mCurrentScene);
		mSceneHierarchy = createSceneHierarchy("Scene", *mCurrentScene);

		return true;
	}

	void EditorApp::draw(float deltaTime)
	{
		Application::draw(deltaTime);

		if (mSceneViewport != nullptr)
		{
			mSceneViewport->drawScene(deltaTime);
		}

		auto& swapChain = mGraphicsDevice->getSwapChain();
		mRenderPass->begin(swapChain);
		mImGuiRenderer->newFrame(*mWindow, deltaTime);

		onGui();
		
		mImGuiRenderer->draw(*mRenderPass);
		mRenderPass->end();
	}

	void EditorApp::setupInput()
	{
		Application::setupInput();
	}

	void EditorApp::onGui()
	{
		ImGui::DockSpaceOverViewport();
		ImGuizmo::SetOrthographic(true);
		ImGuizmo::BeginFrame();
		
		for (auto& widget : mWidgets)
		{
			widget->draw();
		}
	}

	void EditorApp::onMainMenuClick(const std::string& name)
	{
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

	void EditorApp::onSelectNodeClick(Node* selectedNode)
	{
		mInspector->setSelectedNode(selectedNode);
		mSceneViewport->setSelectedNode(selectedNode);		
	}

	void EditorApp::onAssetFileDialogClick(AssetFileDialogType dialogType, AssetFileDialogResult result, const std::string& path)
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
				mSceneHierarchy->setScene(*mCurrentScene);
				mSceneViewport->setScene(*mCurrentScene);
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

	Scene* EditorApp::createDefaultScene(float width, float height, float nearPlane, float farPlane)
	{
		auto scene = std::make_unique<Scene>();
		scene->setName("New Scene");

		auto root = std::make_unique<Node>();
		root->setName("Root");
		scene->setRoot(*root);
		scene->addNode(std::move(root));

		auto* cameraNode = scene->addEmpty();
		cameraNode->setName("Camera");

		scene->addCamera("Camera", glm::vec2{ width, height },  
			nearPlane, farPlane);

		auto* scenePtr = scene.get();
		mResourceCache->addResource(std::move(scene));

		return scenePtr;
	}

	Scene* EditorApp::openScene(const std::string& path)
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

	bool EditorApp::saveScene(Scene* scene, const std::string& path)
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

	MenuBar* EditorApp::createMainMenu()
	{
		auto mainMenu = std::make_unique<MenuBar>();
		mainMenu->onMenuItemClick.subscribe([this](const auto& menuItem) {
			onMainMenuClick(menuItem.name);
		});

		auto* fileMenu = mainMenu->addMenuItem("file", "File");
		auto* editMenu = mainMenu->addMenuItem("edit", "Edit");
		auto* viewMenu = mainMenu->addMenuItem("view", "View");
		auto* toolsMenu = mainMenu->addMenuItem("tools", "Tools");

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
		mainMenu->addMenuItem("spriteEditor", "  Sprite EditorApp  ", "CTRL+T", toolsMenu);

		auto* mainMenuPtr = mainMenu.get();
		mWidgets.push_back(std::move(mainMenu));

		return mainMenuPtr;
	}

	AssetFileDialog* EditorApp::createFileDialog()
	{
		auto fileDialog = std::make_unique<AssetFileDialog>();
		if (!fileDialog->create("/Assets", *mEditorResources))
		{
			LogError("AssetFileDialog::create() failed");
			return nullptr;
		}

		fileDialog->addFileType("Scene File (*.json)", { ".json" });
		fileDialog->onClosed.subscribe([this](auto dialogType, auto result, const auto& path) {
			onAssetFileDialogClick(dialogType, result, path);
		});

		auto* fileDialogPtr = fileDialog.get();
		mWidgets.push_back(std::move(fileDialog));

		return fileDialogPtr;
	}

	MessageBox* EditorApp::createMessageBox()
	{
		auto messageBox = std::make_unique<MessageBox>();
		if (!messageBox->create(500, 250, *mEditorResources))
		{
			LogError("MessageBox::create() failed");
			return nullptr;
		}

		messageBox->onClosed.subscribe([this](auto result) {			
		});

		auto* messageBoxPtr = messageBox.get();
		mWidgets.push_back(std::move(messageBox));

		return messageBoxPtr;
	}

	AssetBrowser* EditorApp::createAssetBrowser(const std::string& title)
	{
		auto assetBrowser = std::make_unique<AssetBrowser>();
		if (!assetBrowser->create("/Assets", *mEditorResources))
		{
			LogError("AssetBrowser::create() failed");
			return nullptr;
		}

		assetBrowser->setTitle(title.c_str());
		assetBrowser->addFileExtension(".*");
		
		auto* assetBrowserPtr = assetBrowser.get();
		mWidgets.push_back(std::move(assetBrowser));

		return assetBrowserPtr;
	}

	SceneViewport* EditorApp::createSceneViewport(const std::string& title, Scene& scene)
	{
		auto sceneViewport = std::make_unique<SceneViewport>();
		if (!sceneViewport->create(mWindow->getWidth(), mWindow->getHeight(), *mEditorResources))
		{
			LogError("Viewport::create() failed");
			return nullptr;
		}

		sceneViewport->setTitle(title);
		sceneViewport->setScene(scene);
		sceneViewport->setCamera(*mEditorCamera);

		auto* sceneViewportPtr = sceneViewport.get();
		mWidgets.push_back(std::move(sceneViewport));

		return sceneViewportPtr;
	}

	Inspector* EditorApp::createInspector(const std::string& title, Scene& scene)
	{
		auto inspector = std::make_unique<Inspector>();
		inspector->setTitle("Inspector");
		inspector->setLayout(*mLayout);
		inspector->setResourceCache(*mEditorResources->getResourceCache());
		inspector->setScene(scene);

		auto* inspectorPtr = inspector.get();
		mWidgets.push_back(std::move(inspector));

		return inspectorPtr;
	}

	SceneHierarchy* EditorApp::createSceneHierarchy(const std::string& title, Scene& scene)
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
}

using namespace Trinity;

int main(int argc, char* argv[])
{
	static EditorApp app;
	app.run({
		.logLevel = LogLevel::Info,
		.title = "Trinity2D - EditorApp",
#ifdef __EMSCRIPTEN__
		.configFile = "/Assets/EditorConfig.json",
#else
		.configFile = "EditorConfig.json",
#endif
	});

	return 0;
}