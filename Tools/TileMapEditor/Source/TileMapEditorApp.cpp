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
#include "Widgets/TileMapViewport.h"
#include "Widgets/TileMapInspector.h"
#include "Widgets/TileMapHierarchy.h"
#include "Widgets/TileSetViewport.h"
#include "Graphics/Texture.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/LineCanvas.h"
#include "Input/Input.h"
#include "TileMap/TileMap.h"
#include "TileMap/TileLayer.h"
#include "TileMap/TileSet.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiFont.h"
#include "TileMap/TileMap.h"
#include "TileMap/TileLayer.h"
#include "VFS/FileSystem.h"

namespace Trinity
{
	bool TileMapEditorApp::init()
	{
		if (!EditorApp::init())
		{
			return false;
		}

		mCurrentTileMap = createDefaultTileMap();
		if (!mCurrentTileMap)
		{
			LogError("TileMapEditorApp::createDefaultTileMap() failed");
			return false;
		}

		mViewport = createTileMapViewport("Viewport", *mCurrentTileMap);
		mInspector = createTileMapInspector("Inspector", *mCurrentTileMap);
		mHierarchy = createTileMapHierarchy("Hierarchy", *mCurrentTileMap);
		mTileSetViewport = createTileSetViewport("TileSet");
		updateTitle();

		auto& input = Input::get();
		auto* keyboard = input.getKeyboard();

		keyboard->onKeyStateUpdated.subscribe([this](auto key, auto pressed) {
			onKeyboardKeyStateUpdated(key, pressed);
		});

		return true;
	}

	void TileMapEditorApp::setupInput()
	{
		EditorApp::setupInput();
	}

	void TileMapEditorApp::onUpdate(float deltaTime)
	{
		if (mViewport != nullptr)
		{
			mViewport->update(deltaTime);
		}

		if (mTileSetViewport != nullptr)
		{
			mTileSetViewport->update(deltaTime);
		}
	}

	void TileMapEditorApp::onDraw(float deltaTime)
	{
		EditorApp::onDraw(deltaTime);

		if (mViewport != nullptr)
		{
			mViewport->drawContent(deltaTime);
		}

		if (mTileSetViewport != nullptr)
		{
			mTileSetViewport->drawContent(deltaTime);
		}
	}

	TileMap* TileMapEditorApp::openTileMap(const std::string& path)
	{
		auto& fileSystem = FileSystem::get();

		auto tileMap = std::make_unique<TileMap>();
		tileMap->setName(fileSystem.getFileName(path, false));
		tileMap->setFileName(path);

		auto jsonStr = fileSystem.readText(path);
		if (jsonStr.empty())
		{
			LogError("FileSystem::readText() failed for: '%s'", path.c_str());
			return nullptr;
		}

		auto* serializer = tileMap->getSerializer();
		json tileMapJson = json::parse(jsonStr);

		if (!serializer->read(tileMapJson, *mResourceCache))
		{
			LogError("TileMapSerializer::read() from json failed for tileMap with path: '%s'", path.c_str());
			return nullptr;
		}

		auto* tileMapPtr = tileMap.get();
		mResourceCache->addResource(std::move(tileMap));

		return tileMapPtr;
	}

	bool TileMapEditorApp::saveTileMap(TileMap* tileMap, const std::string& path)
	{
		if (!tileMap)
		{
			LogError("TileMap is null for path: '%s'", path.c_str());
			return false;
		}

		auto* serializer = tileMap->getSerializer();
		json tileMapJson;

		if (!serializer->write(tileMapJson))
		{
			LogError("TileMapSerializer::write() to json failed for tileMap with path: '%s'", path.c_str());
			return false;
		}

		auto& fileSystem = FileSystem::get();
		if (!fileSystem.writeText(path, tileMapJson.dump(4)))
		{
			LogError("FileSystem::writeText() for tileMap with path: '%s'", path.c_str());
			return false;
		}

		tileMap->setFileName(path);
		tileMap->setName(fileSystem.getFileName(path, false));

		mTileMapChanged = false;
		updateTitle();

		return true;
	}

	void TileMapEditorApp::updateTitle()
	{
		if (mCurrentTileMap != nullptr)
		{
			const auto title = std::format("Trinity2D - TileMap Editor | {}{}", mCurrentTileMap->getName(), 
				mTileMapChanged ? "*" : "");

			mWindow->setTitle(title);
		}
	}

	MenuBar* TileMapEditorApp::createMainMenu()
	{
		auto* mainMenu = EditorApp::createMainMenu();
		auto* fileMenu = mainMenu->addMenuItem("file", "File");
		auto* editMenu = mainMenu->addMenuItem("edit", "Edit");
		auto* viewMenu = mainMenu->addMenuItem("view", "View");

		mainMenu->addMenuItem("newTileMap", "  New TileMap  ", "CTRL+N", fileMenu);
		mainMenu->addMenuItem("openTileMap", "  Open TileMap  ", "CTRL+O", fileMenu);
		mainMenu->addSeparator(fileMenu);
		mainMenu->addMenuItem("saveTileMap", "  Save TileMap  ", "CTRL+S", fileMenu);
		mainMenu->addMenuItem("saveAsTileMap", "  Save TileMap As  ", "CTRL+SHIFT+S", fileMenu);
		mainMenu->addSeparator(fileMenu);
		mainMenu->addMenuItem("exit", "  Exit  ", "ALT+X", fileMenu);

		mainMenu->addMenuItem("undo", "  Undo  ", "CTRL+Z", editMenu);
		mainMenu->addMenuItem("redo", "  Redo  ", "CTRL+Y", editMenu);
		mainMenu->addSeparator(editMenu);
		mainMenu->addMenuItem("cut", "  Cut  ", "CTRL+X", editMenu);
		mainMenu->addMenuItem("copy", "  Copy  ", "CTRL+C", editMenu);
		mainMenu->addMenuItem("paste", "  Paste  ", "CTRL+V", editMenu);
		mainMenu->addMenuItem("inspector", "  Inspector  ", "CTRL+I", viewMenu);

		return mainMenu;
	}

	AssetFileDialog* TileMapEditorApp::createFileDialog()
	{
		auto* fileDialog = EditorApp::createFileDialog();
		fileDialog->addFileType("TileMap File (*.json)", { ".json" });

		return fileDialog;
	}

	TileMap* TileMapEditorApp::createDefaultTileMap() const
	{
		auto tileMap = std::make_unique<TileMap>();
		tileMap->setName("New TileMap");

		auto* tileMapPtr = tileMap.get();
		mResourceCache->addResource(std::move(tileMap));

		return tileMapPtr;
	}

	TileMapViewport* TileMapEditorApp::createTileMapViewport(const std::string& title, TileMap& tileMap)
	{
		auto viewport = std::make_unique<TileMapViewport>();
		if (!viewport->create(*mResources))
		{
			LogError("TileMapViewport::create() failed");
			return nullptr;
		}

		viewport->setTitle(title);
		viewport->setTileMap(tileMap);

		viewport->onResize.subscribe([this](auto width, auto height) {
			onViewportResize(width, height);
		});

		viewport->onTileMapEdit.subscribe([this]() {
			onTileMapEditClick();
		});

		auto* resolution = viewport->getSelectedResolution();
		viewport->resize(resolution->width, resolution->height);

		auto* viewportPtr = viewport.get();
		mWidgets.push_back(std::move(viewport));

		return viewportPtr;
	}

	TileMapInspector* TileMapEditorApp::createTileMapInspector(const std::string& title, TileMap& tileMap)
	{
		auto inspector = std::make_unique<TileMapInspector>();
		inspector->setTitle(title);
		inspector->setLayout(*mLayout);
		inspector->setResourceCache(*mResourceCache);
		inspector->setTileMap(*mCurrentTileMap);

		auto* inspectorPtr = inspector.get();
		mWidgets.push_back(std::move(inspector));

		return inspectorPtr;
	}

	TileMapHierarchy* TileMapEditorApp::createTileMapHierarchy(const std::string& title, TileMap& tileMap)
	{
		auto hierarchy = std::make_unique<TileMapHierarchy>();
		hierarchy->setTitle(title);
		hierarchy->setTileMap(tileMap);

		hierarchy->onSelectTileLayer.subscribe([this](auto tileLayer) {
			onSelectTileLayerClick(tileLayer);
		});

		hierarchy->onSelectTileSet.subscribe([this](auto tileSet) {
			onSelectTileSetClick(tileSet);
		});

		auto* spriteHierarchyPtr = hierarchy.get();
		mWidgets.push_back(std::move(hierarchy));

		return spriteHierarchyPtr;
	}

	TileSetViewport* TileMapEditorApp::createTileSetViewport(const std::string& title)
	{
		auto viewport = std::make_unique<TileSetViewport>();
		if (!viewport->create(*mResources))
		{
			LogError("TileSetViewport::create() failed");
			return nullptr;
		}

		viewport->setTitle(title);
		viewport->onResize.subscribe([this](auto width, auto height) {
			onTileSetViewportResize(width, height);
		});

		viewport->onSelectTiles.subscribe([this](const auto& minTileIndex, const auto& maxTileIndex, const auto& firstTileIndex) {
			onSelectTilesClick(minTileIndex, maxTileIndex, firstTileIndex);
		});

		auto* resolution = viewport->getSelectedResolution();
		viewport->resize(resolution->width, resolution->height);

		auto* viewportPtr = viewport.get();
		mWidgets.push_back(std::move(viewport));

		return viewportPtr;
	}

	void TileMapEditorApp::onMainMenuClick(const std::string& name)
	{
		EditorApp::onMainMenuClick(name);

		if (name == "exit")
		{
			exit();
		}
		else if (name == "newTileMap")
		{
			if (mCurrentTileMap != nullptr)
			{
				mResourceCache->removeResource(mCurrentTileMap);
			}

			mCurrentTileMap = createDefaultTileMap();
			mViewport->setTileMap(*mCurrentTileMap);
			mInspector->setTileMap(*mCurrentTileMap);
			mHierarchy->setTileMap(*mCurrentTileMap);

			onSelectTileLayerClick(0);
			onSelectTileSetClick(0);
		}
		else if (name == "openTileMap")
		{
			if (mFileDialog != nullptr)
			{
				mFileDialog->show(AssetFileDialogType::Open, "Open TileMap");
			}
		}
		else if (name == "saveTileMap")
		{
			if (mTileMapChanged && mCurrentTileMap != nullptr)
			{
				if (mCurrentTileMap->getFileName().empty())
				{
					if (mFileDialog != nullptr)
					{
						mFileDialog->show(AssetFileDialogType::Save, "Save TileMap");
					}
				}
				else
				{
					saveTileMap(mCurrentTileMap, mCurrentTileMap->getFileName());
				}
			}
		}
		else if (name == "saveAsTileMap")
		{
			if (mFileDialog != nullptr && mCurrentTileMap != nullptr)
			{
				mFileDialog->show(AssetFileDialogType::Save, "Save TileMap As");
			}
		}
		else if (name == "cut")
		{
			mViewport->cut();
		}
		else if (name == "copy")
		{
			mViewport->copy();
		}
		else if (name == "paste")
		{
			mViewport->paste();
		}
		else if (name == "undo")
		{
			mViewport->undo();
		}
		else if (name == "redo")
		{
			mViewport->redo();
		}
	}

	void TileMapEditorApp::onAssetFileDialogClick(AssetFileDialogType dialogType, AssetFileDialogResult result, const std::string& path)
	{
		if (result == AssetFileDialogResult::Cancel)
		{
			return;
		}

		if (dialogType == AssetFileDialogType::Open)
		{
			auto* tileMap = openTileMap(path);
			if (tileMap != nullptr)
			{
				if (mCurrentTileMap != nullptr)
				{
					mResourceCache->removeResource(mCurrentTileMap);
				}

				mCurrentTileMap = tileMap;
				mViewport->setTileMap(*mCurrentTileMap);
				mInspector->setTileMap(*mCurrentTileMap);
				mHierarchy->setTileMap(*mCurrentTileMap);

				onSelectTileLayerClick(0);
				onSelectTileSetClick(0);
			}
			else
			{
				mMessageBox->show(std::format("Unable to open TileMap file '{}'", path), "Error",
					MessageBoxButtons::Ok, MessageBoxIcon::Error);
			}
		}
		else if (dialogType == AssetFileDialogType::Save || dialogType == AssetFileDialogType::SaveAs)
		{
			if (!saveTileMap(mCurrentTileMap, path))
			{
				mMessageBox->show(std::format("Unable to save TileMap file '{}'", path), "Error",
					MessageBoxButtons::Ok, MessageBoxIcon::Error);

				return;
			}

			if (mAssetBrowser != nullptr)
			{
				auto& fileSystem = FileSystem::get();
				mAssetBrowser->refreshPath(fileSystem.getDirectory(path));
			}

			mTileMapChanged = false;
			updateTitle();
		}
	}

	void TileMapEditorApp::onKeyboardKeyStateUpdated(int32_t key, bool pressed)
	{
		if (!pressed)
		{
			auto& input = Input::get();
			auto* keyboard = input.getKeyboard();
			auto ctrlKeyDown = keyboard->isKeyDown(KEY_LEFT_CONTROL);
			auto shiftKeyDown = keyboard->isKeyDown(KEY_LEFT_SHIFT);
			auto altKeyDown = keyboard->isKeyDown(KEY_LEFT_ALT);

			if (key == KEY_O && ctrlKeyDown)
			{
				if (mFileDialog != nullptr)
				{
					mFileDialog->show(AssetFileDialogType::Open, "Open TileMap");
				}
			}
			else if (key == KEY_S && ctrlKeyDown)
			{
				if (mTileMapChanged && mCurrentTileMap != nullptr)
				{
					if (mCurrentTileMap->getFileName().empty())
					{
						if (mFileDialog != nullptr)
						{
							mFileDialog->show(AssetFileDialogType::Save, "Save TileMap");
						}
					}
					else
					{
						saveTileMap(mCurrentTileMap, mCurrentTileMap->getFileName());
					}
				}
			}
			else if (key == KEY_S && ctrlKeyDown && shiftKeyDown)
			{
				if (mFileDialog != nullptr && mCurrentTileMap != nullptr)
				{
					mFileDialog->show(AssetFileDialogType::Save, "Save TileMap As");
				}
			}
			else if (key == KEY_X && ctrlKeyDown)
			{
				if (mViewport != nullptr)
				{
					mViewport->cut();
				}
			}
			else if (key == KEY_C && ctrlKeyDown)
			{
				if (mViewport != nullptr)
				{
					mViewport->copy();
				}
			}
			else if (key == KEY_V && ctrlKeyDown)
			{
				if (mViewport != nullptr)
				{
					mViewport->paste();
				}
			}
			else if (key == KEY_Z && ctrlKeyDown)
			{
				if (mViewport != nullptr)
				{
					mViewport->undo();
				}
			}
			else if (key == KEY_Y && ctrlKeyDown)
			{
				if (mViewport != nullptr)
				{
					mViewport->redo();
				}
			}
			else if (key == KEY_X && altKeyDown)
			{
				exit();
			}
		}
	}

	void TileMapEditorApp::onViewportResize(uint32_t width, uint32_t height)
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

	void TileMapEditorApp::onTileSetViewportResize(uint32_t width, uint32_t height)
	{
		if (mTileSetViewport != nullptr)
		{
			auto* frameBuffer = mTileSetViewport->getFrameBuffer();
			auto* colorTarget = frameBuffer->getColorTexture();

			if (colorTarget != nullptr)
			{
				mImGuiRenderer->invalidateTexture(*colorTarget);
			}
		}
	}

	void TileMapEditorApp::onSelectTileLayerClick(uint32_t selectedTileLayer)
	{
		if (mViewport != nullptr)
		{
			mViewport->setSelectedTileLayer(selectedTileLayer);
		}
	}

	void TileMapEditorApp::onSelectTileSetClick(uint32_t selectedTileSet)
	{
		if (mInspector != nullptr)
		{
			mInspector->setSelectedTileSet(selectedTileSet);
		}

		if (mTileSetViewport != nullptr)
		{
			auto* tileSet = mCurrentTileMap->getTileSet(selectedTileSet);
			if (tileSet != nullptr)
			{
				mTileSetViewport->setTitle(tileSet->getName());
			}
			else
			{
				mTileSetViewport->setTitle("Tile Set");
			}

			mTileSetViewport->setTileSet(tileSet);
		}

		if (mViewport != nullptr)
		{
			mViewport->setSelectedTileSet(selectedTileSet);
		}
	}

	void TileMapEditorApp::onTileMapEditClick()
	{
		mTileMapChanged = true;
		updateTitle();
	}

	void TileMapEditorApp::onSelectTilesClick(const glm::ivec2& minTileIndex, const glm::ivec2& maxTileIndex, 
		const glm::ivec2& firstTileIndex)
	{
		if (mViewport != nullptr)
		{
			mViewport->setSelectedTiles(minTileIndex, maxTileIndex, firstTileIndex);
		}

		if (mInspector != nullptr)
		{
			mInspector->setSelectedTile(firstTileIndex);
		}
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