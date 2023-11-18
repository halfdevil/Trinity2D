#include "EditorApp.h"
#include "Widgets/MenuBar.h"
#include "Widgets/AssetBrowser.h"
#include "Widgets/SceneHierarchy.h"
#include "Widgets/Inspector.h"
#include "Widgets/Viewport.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiFont.h"
#include "Scene/Scene.h"
#include "Scene/Components/Camera.h"
#include "Scene/ComponentFactory.h"
#include "Scene/TextureRenderer.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderPass.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/BatchRenderer.h"
#include "VFS/FileSystem.h"
#include "Editor/EditorLayout.h"
#include "Core/EditorResources.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "Core/Debugger.h"
#include "Core/Clock.h"
#include "Core/Window.h"

namespace Trinity
{
	bool EditorApp::init()
	{
		if (!Application::init())
		{
			return false;
		}

		auto& swapChain = mGraphicsDevice->getSwapChain();

		mRenderPass = std::make_unique<RenderPass>();
		mGraphicsDevice->setClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });

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

		auto sceneViewport = std::make_unique<Viewport>();
		sceneViewport->setTitle("Scene Viewport");

		if (!sceneViewport->create(mWindow->getWidth(), mWindow->getHeight(), *mEditorResources))
		{
			LogError("Viewport::create() failed");
			return false;
		}

		auto mainMenu = std::make_unique<MenuBar>();
		mainMenu->onMenuItemClick.subscribe([this](const auto& menuItem) {
			onMainMenuClick(menuItem.title);
		});

		auto* fileMenu = mainMenu->addMenuItem("file", "File");
		auto* toolsMenu = mainMenu->addMenuItem("tools", "Tools");

		mainMenu->addMenuItem("newProject", "New Project", "CTRL+N", fileMenu);
		mainMenu->addMenuItem("openProject", "Open Project", "CTRL+O", fileMenu);
		mainMenu->addMenuItem("spriteEditor", "Sprite EditorApp", "CTRL+T", toolsMenu);

		auto assetBrowser = std::make_unique<AssetBrowser>();
		assetBrowser->setTitle("Asset Browser");

		if (!assetBrowser->create("/Assets", *mEditorResources))
		{
			LogError("AssetBrowser::create() failed");
			return false;
		}

		auto addNode = [this](const std::string& name, Node* parent = nullptr) {
			auto node = std::make_unique<Node>();
			node->setName(name);

			if (parent != nullptr)
			{
				parent->addChild(*node);
			}
			else
			{
				mTestScene->addChild(*node);
			}

			auto* nodePtr = node.get();
			mTestScene->addNode(std::move(node));

			return nodePtr;
		};

		auto texture = std::make_unique<Texture>();
		if (!texture->create("/Assets/Editor/Textures/grass.png", wgpu::TextureFormat::RGBA8Unorm))
		{
			LogError("AssetBrowser::create() failed");
			return false;
		}

		auto scene = std::make_unique<Scene>();
		scene->setName("Test Scene");

		mTestScene = scene.get();
		mEditorResources->getResourceCache()->addResource(std::move(scene));

		auto root = std::make_unique<Node>();
		root->setName("Root");

		mTestScene->setRoot(*root);
		mTestScene->addNode(std::move(root));
		
		auto* node1 = addNode("Node1");
		auto* node2 = addNode("Node2");
		auto* node3 = addNode("Node3");

		addNode("Node11", node1);
		addNode("Node12", node1);
		addNode("Node13", node1);

		addNode("Node21", node2);
		addNode("Node22", node2);
		addNode("Node23", node2);

		addNode("Node31", node3);
		addNode("Node32", node3);
		addNode("Node33", node3);

		mTestScene->addCamera(
			"mainCamera",
			{ (float)mWindow->getWidth(), (float)mWindow->getHeight() },
			0.1f,
			100.0f
		);

		mTestScene->addTextureRenderable(
			*texture,
			"Node1"
		);

		auto& transform = node1->getTransform();
		transform.setTranslation({ -256.0f, -256.0f, 1.0f });

		auto sceneHierarchy = std::make_unique<SceneHierarchy>();
		sceneHierarchy->setTitle("Scene");
		sceneHierarchy->setScene(*mTestScene);

		auto inspector = std::make_unique<Inspector>();
		inspector->setTitle("Inspector");

		mTextureRenderer = std::make_unique<TextureRenderer>();
		if (!mTextureRenderer->create(*sceneViewport->getFrameBuffer()))
		{
			LogError("TextureRenderer::create() failed!!");
			return false;
		}
		
		mTextureRenderer->setScene(*mTestScene, "mainCamera");
		mSceneViewport = sceneViewport.get();
		mSceneHierarchy = sceneHierarchy.get();
		mInspector = inspector.get();

		mEditorResources->getResourceCache()->addResource(std::move(texture));
		mWidgets.push_back(std::move(mainMenu));
		mWidgets.push_back(std::move(assetBrowser));
		mWidgets.push_back(std::move(sceneHierarchy));
		mWidgets.push_back(std::move(sceneViewport));
		mWidgets.push_back(std::move(inspector));

		return true;
	}

	void EditorApp::draw(float deltaTime)
	{
		Application::draw(deltaTime);

		if (mSceneViewport != nullptr)
		{
			mRenderPass->begin(*mSceneViewport->getFrameBuffer());
			mTextureRenderer->draw(*mRenderPass);
			mRenderPass->end();
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

		mInput->bindAction("exit", InputEvent::Pressed, [this](int32_t key) {
			exit();
		});
	}

	void EditorApp::onGui()
	{
		ImGui::DockSpaceOverViewport();
		
		for (auto& widget : mWidgets)
		{
			mInspector->setSelectedNode(mSceneHierarchy->getCurrentNode());
			widget->draw();
		}
	}

	void EditorApp::onMainMenuClick(const std::string& title)
	{
		LogInfo("MainMenuItem clicked: %s", title.c_str());
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