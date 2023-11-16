#include "Editor.h"
#include "EditorResources.h"
#include "Widgets/EditorMenu.h"
#include "Widgets/AssetBrowser.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiFont.h"
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
		mGraphicsDevice->setClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });

		mEditorResources = std::make_unique<EditorResources>();
		if (!mEditorResources->create())
		{
			LogError("EditorCache::create() failed");
			return false;
		}

		mImGuiRenderer = std::make_unique<ImGuiRenderer>();
		if (!mImGuiRenderer->create(*mWindow, swapChain))
		{
			LogError("ImGuiRenderer::create() failed!!");
			return false;
		}

		if (!loadFonts())
		{
			LogError("Editor::loadFonts() failed");
			return false;
		}

		auto mainMenu = std::make_unique<EditorMenu>();
		mainMenu->setMainMenu(true);

		mainMenu->onMenuItemClick.subscribe([this](const auto& menuItem) {
			onMainMenuClick(menuItem.title);
		});

		auto* fileMenu = mainMenu->addMenuItem("file", "File");
		auto* toolsMenu = mainMenu->addMenuItem("tools", "Tools");

		mainMenu->addMenuItem("newProject", "New Project", "CTRL+N", fileMenu);
		mainMenu->addMenuItem("openProject", "Open Project", "CTRL+O", fileMenu);
		mainMenu->addMenuItem("spriteEditor", "Sprite Editor", "CTRL+T", toolsMenu);

		auto assetBrowser = std::make_unique<AssetBrowser>();
		if (!assetBrowser->create("/Assets", *mEditorResources))
		{
			LogError("AssetBrowser::create() failed");
			return false;
		}

		assetBrowser->setTitle("Asset Browser");
		mWidgets.push_back(std::move(mainMenu));
		mWidgets.push_back(std::move(assetBrowser));

		return true;
	}

	void Editor::draw(float deltaTime)
	{
		Application::draw(deltaTime);

		auto& swapChain = mGraphicsDevice->getSwapChain();
		mRenderPass->begin(swapChain);

		mImGuiRenderer->newFrame(*mWindow, deltaTime);
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

	bool Editor::loadFonts()
	{
		if (!mConfig.contains("font"))
		{
			return false;
		}

		auto fontConfig = mConfig["font"];
		auto fontFile = fontConfig["file"].get<std::string>();
		auto fontSizes = fontConfig["sizes"].get<std::vector<float>>();

		for (uint32_t idx = 0; idx < (uint32_t)fontSizes.size(); idx++)
		{
			auto fontSize = fontSizes[idx];
			auto fontName = "font-" + std::to_string((uint32_t)fontSize);

			auto font = mEditorResources->loadFont(fontName, fontFile, fontSize, false);
			if (!font)
			{
				LogError("EditorResource::loadFont() failed for: '%s'", fontFile.c_str());
				return false;
			}

			if (fontConfig.contains("icon"))
			{
				auto iconFontFile = fontConfig["icon"].get<std::string>();
				if (!font->mergeFont(iconFontFile, kIconFontRanges))
				{
					LogError("ImGuiFont::mergeFont() failed for: '%s'", iconFontFile.c_str());
					return false;
				}
			}

			if (!font->build())
			{
				LogError("ImGuiFont::build() failed for: '%s'", fontFile.c_str());
				return false;
			}
		}

		return true;
	}

	void Editor::onGui()
	{
		ImGui::DockSpaceOverViewport();
		
		for (auto& widget : mWidgets)
		{
			widget->draw();
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