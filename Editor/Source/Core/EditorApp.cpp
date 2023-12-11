#include "Core/EditorApp.h"
#include "Core/EditorLayout.h"
#include "Core/EditorTheme.h"
#include "Core/EditorResources.h"
#include "Core/EditorGizmo.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "Core/Debugger.h"
#include "Core/Clock.h"
#include "Core/Window.h"
#include "Editor/EditorLayout.h"
#include "Widgets/AssetFileDialog.h"
#include "Widgets/MenuBar.h"
#include "Widgets/AssetBrowser.h"
#include "Widgets/MessageBox.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiFont.h"
#include "Scene/Scene.h"
#include "Scene/Components/Camera.h"
#include "Graphics/GraphicsDevice.h"
#include "VFS/FileSystem.h"
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

		mResources = std::make_unique<EditorResources>();
		if (!mResources->create(mConfig["resources"], *mResourceCache))
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

		mLayout = std::make_unique<EditorLayout>();
		if (!mLayout->updateFiles("/Assets", FileType::Texture))
		{
			LogError("EditorLayout::updateFiles() failed for 'texture'");
			return false;
		}

		mMainMenu = createMainMenu();
		mFileDialog = createFileDialog();
		mMessageBox = createMessageBox();
		mAssetBrowser = createAssetBrowser(ICON_FA_GLOBE " Asset Browser");

		return true;
	}

	void EditorApp::update(float deltaTime)
	{
		Application::update(deltaTime);
		onUpdate(deltaTime);
	}

	void EditorApp::draw(float deltaTime)
	{
		Application::draw(deltaTime);

		onDraw(deltaTime);
		onPreGui(deltaTime);
		onGui();
		onPostGui();
	}

	void EditorApp::setupInput()
	{
		Application::setupInput();
	}

	void EditorApp::onUpdate(float deltaTime)
	{
	}

	void EditorApp::onDraw(float deltaTime)
	{
	}

	void EditorApp::onPreGui(float deltaTime)
	{
		auto& swapChain = mGraphicsDevice->getSwapChain();
		mRenderPass->begin(swapChain);
		mImGuiRenderer->newFrame(*mWindow, deltaTime);
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

	void EditorApp::onPostGui()
	{
		mImGuiRenderer->draw(*mRenderPass);
		mRenderPass->end();
	}

	void EditorApp::onMainMenuClick(const std::string& name)
	{		
	}

	void EditorApp::onAssetFileDialogClick(AssetFileDialogType dialogType, AssetFileDialogResult result, const std::string& path)
	{		
	}

	Scene* EditorApp::createDefaultScene(float width, float height)
	{
		auto scene = std::make_unique<Scene>();
		scene->setName("New Scene");

		auto root = std::make_unique<Node>();
		root->setName("Root");
		scene->setRoot(*root);
		scene->addNode(std::move(root));

		auto* cameraNode = scene->addEmpty();
		cameraNode->setName("Camera");
		scene->addCamera("Camera", glm::vec2{ width, height });

		auto* scenePtr = scene.get();
		mResourceCache->addResource(std::move(scene));

		return scenePtr;
	}

	MenuBar* EditorApp::createMainMenu()
	{
		auto mainMenu = std::make_unique<MenuBar>();
		mainMenu->onMenuItemClick.subscribe([this](const auto& menuItem) {
			onMainMenuClick(menuItem.name);
		});

		auto* mainMenuPtr = mainMenu.get();
		mWidgets.push_back(std::move(mainMenu));

		return mainMenuPtr;
	}

	AssetFileDialog* EditorApp::createFileDialog()
	{
		auto fileDialog = std::make_unique<AssetFileDialog>();
		if (!fileDialog->create("/Assets", *mResources))
		{
			LogError("AssetFileDialog::create() failed");
			return nullptr;
		}

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
		if (!messageBox->create(500, 250, *mResources))
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
		if (!assetBrowser->create("/Assets", *mResources))
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
}