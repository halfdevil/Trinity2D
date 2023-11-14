#include "Editor.h"
#include "Widgets/EditorMenu.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "Gui/Font.h"
#include "Gui/TextRenderer.h"
#include "Scene/Scene.h"
#include "Scene/Components/Camera.h"
#include "Scene/ComponentFactory.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderPass.h"
#include "Graphics/GraphicsDevice.h"
#include "VFS/FileSystem.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "Core/Debugger.h"
#include "Core/Clock.h"
#include "Core/Window.h"

namespace Trinity
{
	bool Editor::init()
	{
		if (!Application::init())
		{
			return false;
		}

		auto& swapChain = mGraphicsDevice->getSwapChain();
		mRenderPass = std::make_unique<RenderPass>();

		mImGuiRenderer = std::make_unique<ImGuiRenderer>();
		if (!mImGuiRenderer->create(*mWindow, "/Assets/Fonts/CascadiaCode.ttf", swapChain))
		{
			LogError("Gui::create() failed!!");
			return false;
		}

		mMainMenu = std::make_unique<EditorMenu>();
		mMainMenu->setMainMenu(true);
		mMainMenu->onMenuItemClick.subscribe([this](const auto& menuItem) {
			onMainMenuClick(menuItem.title);
		});

		auto* fileMenu = mMainMenu->addMenuItem("File");
		auto* toolsMenu = mMainMenu->addMenuItem("Tools");

		mMainMenu->addMenuItem("New Project", "CTRL+N", fileMenu);
		mMainMenu->addMenuItem("Open Project", "CTRL+O", fileMenu);
		mMainMenu->addMenuItem("Sprite Editor", "CTRL+T", toolsMenu);

		mGraphicsDevice->setClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });
		return true;
	}

	void Editor::draw(float deltaTime)
	{
		Application::draw(deltaTime);

		auto& swapChain = mGraphicsDevice->getSwapChain();
		mRenderPass->begin(swapChain);

		mImGuiRenderer->newFrame(*mWindow, mClock->getDeltaTime());
		onGui();
		mImGuiRenderer->draw(*mRenderPass);
		mRenderPass->end();
	}

	void Editor::setupInput()
	{
		Application::setupInput();

		mInput->bindAction("exit", InputEvent::Pressed, [this](int32_t key) {
			exit();
		});
	}

	void Editor::onGui()
	{
		ImGui::DockSpaceOverViewport();
		if (mMainMenu != nullptr)
		{
			mMainMenu->draw();
		}
	}

	void Editor::onMainMenuClick(const std::string& title)
	{
		LogInfo("MainMenuItem clicked: %s", title.c_str());
	}
}

using namespace Trinity;

int main(int argc, char* argv[])
{
	static Editor app;
	app.run({
		.logLevel = LogLevel::Info,
		.title = "Trinity2D - Editor",
#ifdef __EMSCRIPTEN__
		.configFile = "/Assets/EditorConfig.json",
#else
		.configFile = "EditorConfig.json",
#endif
	});

	return 0;
}