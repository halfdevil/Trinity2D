#include "TileMapEditorApp.h"
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
#include "Widgets/MessageBox.h"
#include "Graphics/Texture.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/LineCanvas.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiFont.h"
#include "TileMap/TileMap.h"
#include "TileMap/TileLayer.h"
#include "VFS/FileSystem.h"

namespace Trinity
{
	bool TileMapEditorApp::init()
	{
		return false;
	}

	void TileMapEditorApp::setupInput()
	{
		EditorApp::setupInput();
	}

	void TileMapEditorApp::onUpdate(float deltaTime)
	{
	}

	void TileMapEditorApp::onDraw(float deltaTime)
	{
	}
}

using namespace Trinity;

int main(int argc, char* argv[])
{
	static TileMapEditorApp app;
	app.run({
		.logLevel = LogLevel::Info,
		.title = "Trinity2D - TileMap Editor",
#ifdef __EMSCRIPTEN__
		.configFile = "/Assets/EditorConfig.json",
#else
		.configFile = "EditorConfig.json",
#endif
		});

	return 0;
}