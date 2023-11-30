#include "SpriteEditorApp.h"
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
#include "Scene/ComponentFactory.h"
#include "Graphics/Texture.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/BatchRenderer.h"
#include "VFS/FileSystem.h"
#include "ImGuizmo.h"
#include "IconsFontAwesome6.h"
#include <format>

namespace Trinity
{
	bool SpriteEditorApp::init()
	{
		if (!EditorApp::init())
		{
			return false;
		}

		return true;
	}

	void SpriteEditorApp::setupInput()
	{
		Application::setupInput();
	}

	void SpriteEditorApp::onDraw(float deltaTime)
	{
		EditorApp::onDraw(deltaTime);
	}

	MenuBar* SpriteEditorApp::createMainMenu()
	{
		auto* mainMenu = EditorApp::createMainMenu();
		auto* fileMenu = mainMenu->addMenuItem("file", "File");
		auto* editMenu = mainMenu->addMenuItem("edit", "Edit");
		auto* viewMenu = mainMenu->addMenuItem("view", "View");

		mainMenu->addMenuItem("newSprite", "  New Sprite  ", "CTRL+N", fileMenu);
		mainMenu->addMenuItem("openSprite", "  Open Sprite  ", "CTRL+O", fileMenu);
		mainMenu->addSeparator(fileMenu);
		mainMenu->addMenuItem("saveSprite", "  Save Sprite  ", "CTRL+S", fileMenu);
		mainMenu->addMenuItem("saveAsSprite", "  Save Sprite As  ", "CTRL+SHIFT+S", fileMenu);
		mainMenu->addSeparator(fileMenu);
		mainMenu->addMenuItem("exit", "  Exit  ", "ALT+F4", fileMenu);

		mainMenu->addMenuItem("cut", "  Cut  ", "CTRL+X", editMenu);
		mainMenu->addMenuItem("copy", "  Copy  ", "CTRL+C", editMenu);
		mainMenu->addMenuItem("paste", "  Paste  ", "CTRL+V", editMenu);
		mainMenu->addMenuItem("inspector", "  Inspector  ", "CTRL+I", viewMenu);

		return mainMenu;
	}

	AssetFileDialog* SpriteEditorApp::createFileDialog()
	{
		auto* fileDialog = EditorApp::createFileDialog();
		fileDialog->addFileType("Sprite File (*.json)", { ".json" });

		return fileDialog;
	}
}

using namespace Trinity;

int main(int argc, char* argv[])
{
	static SpriteEditorApp app;
	app.run({
		.logLevel = LogLevel::Info,
		.title = "Trinity2D - Sprite Editor",
#ifdef __EMSCRIPTEN__
		.configFile = "/Assets/EditorConfig.json",
#else
		.configFile = "EditorConfig.json",
#endif
	});

	return 0;
}