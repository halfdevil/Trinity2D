#include "SpriteEditorApp.h"
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
#include "Widgets/SpriteHierarchy.h"
#include "Widgets/SpriteInspector.h"
#include "Widgets/SpriteViewport.h"
#include "Widgets/SpriteAnimationPlayer.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiFont.h"
#include "Scene/Scene.h"
#include "Scene/SceneSystem.h"
#include "Scene/Components/Camera.h"
#include "Scene/ComponentFactory.h"
#include "Scene/Sprite.h"
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
	bool SpriteEditorApp::init()
	{
		if (!EditorApp::init())
		{
			return false;
		}

		mCurrentSprite = createDefaultSprite();
		if (!mCurrentSprite)
		{
			LogError("SpriteEditorApp::createDefaultSprite() failed");
			return false;
		}

		mHierarchy = createSpriteHierarchy("Sprite", *mCurrentSprite);
		mInspector = createSpriteInspector("Inspector", *mCurrentSprite);
		mViewport = createSpriteViewport(mCurrentSprite->getName(), *mCurrentSprite);
		mAnimPlayer = createSpriteAnimationPlayer("Animation Player", *mCurrentSprite);

		return true;
	}

	void SpriteEditorApp::setupInput()
	{
		EditorApp::setupInput();
	}

	void SpriteEditorApp::onUpdate(float deltaTime)
	{
		if (mViewport != nullptr)
		{
			mViewport->update(deltaTime);
		}

		if (mAnimPlayer != nullptr)
		{
			mAnimPlayer->update(deltaTime);
		}
	}

	void SpriteEditorApp::onDraw(float deltaTime)
	{
		EditorApp::onDraw(deltaTime);

		if (mViewport != nullptr)
		{
			mViewport->drawContent(deltaTime);
		}

		if (mAnimPlayer != nullptr)
		{
			mAnimPlayer->drawContent(deltaTime);
		}
	}

	Sprite* SpriteEditorApp::openSprite(const std::string& path)
	{
		auto& fileSystem = FileSystem::get();

		auto sprite = std::make_unique<Sprite>();
		sprite->setName(fileSystem.getFileName(path, false));
		sprite->setFileName(path);

		auto jsonStr = fileSystem.readText(path);
		if (jsonStr.empty())
		{
			LogError("FileSystem::readText() failed for: '%s'", path.c_str());
			return nullptr;
		}

		auto* serializer = sprite->getSerializer();
		json spriteJson = json::parse(jsonStr);

		if (!serializer->read(spriteJson, *mResourceCache))
		{
			LogError("SpriteSerializer::read() from json failed for sprite with path: '%s'", path.c_str());
			return nullptr;
		}

		auto* spritePtr = sprite.get();
		mResourceCache->addResource(std::move(sprite));

		return spritePtr;
	}

	bool SpriteEditorApp::saveSprite(Sprite* sprite, const std::string& path)
	{
		if (!sprite)
		{
			LogError("Sprite is null for path: '%s'", path.c_str());
			return false;
		}

		auto* serializer = sprite->getSerializer();
		json spriteJson;

		if (!serializer->write(spriteJson))
		{
			LogError("SpriteSerializer::write() to json failed for sprite with path: '%s'", path.c_str());
			return false;
		}

		auto& fileSystem = FileSystem::get();
		if (!fileSystem.writeText(path, spriteJson.dump(4)))
		{
			LogError("FileSystem::writeText() for sprite with path: '%s'", path.c_str());
			return false;
		}

		return true;
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

	Sprite* SpriteEditorApp::createDefaultSprite() const
	{
		auto sprite = std::make_unique<Sprite>();
		sprite->setName("New Sprite");

		auto* spritePtr = sprite.get();
		mResourceCache->addResource(std::move(sprite));

		return spritePtr;
	}

	SpriteHierarchy* SpriteEditorApp::createSpriteHierarchy(const std::string& title, Sprite& sprite)
	{
		auto hierarchy = std::make_unique<SpriteHierarchy>();
		hierarchy->setTitle(title);
		hierarchy->setSprite(sprite);

		hierarchy->onSelectFrame.subscribe([this](auto frame) {
			onSelectFrameHierarchyClick(frame);
		});

		hierarchy->onSelectAnimation.subscribe([this](auto animation) {
			onSelectAnimationClick(animation);
		});

		auto* spriteHierarchyPtr = hierarchy.get();
		mWidgets.push_back(std::move(hierarchy));

		return spriteHierarchyPtr;
	}

	SpriteInspector* SpriteEditorApp::createSpriteInspector(const std::string& title, Sprite& sprite)
	{
		auto inspector = std::make_unique<SpriteInspector>();
		inspector->setTitle(title);
		inspector->setLayout(*mLayout);
		inspector->setResourceCache(*mResourceCache);
		inspector->setSprite(*mCurrentSprite);

		auto* inspectorPtr = inspector.get();
		mWidgets.push_back(std::move(inspector));

		return inspectorPtr;
	}

	SpriteViewport* SpriteEditorApp::createSpriteViewport(const std::string& title, Sprite& sprite)
	{
		auto viewport = std::make_unique<SpriteViewport>();
		if (!viewport->create(*mResources))
		{
			LogError("SpriteViewport::create() failed");
			return nullptr;
		}

		viewport->setTitle(title);
		viewport->setSprite(sprite);

		viewport->onSelectFrame.subscribe([this](auto selectedFrame) {
			onSelectFrameViewportClick(selectedFrame);
		});

		viewport->onResize.subscribe([this](auto width, auto height) {
			onViewportResize(width, height);
		});

		auto* resolution = viewport->getSelectedResolution();
		viewport->resize(resolution->width, resolution->height);

		auto* viewportPtr = viewport.get();
		mWidgets.push_back(std::move(viewport));

		return viewportPtr;
	}

	SpriteAnimationPlayer* SpriteEditorApp::createSpriteAnimationPlayer(const std::string& title, Sprite& sprite)
	{
		auto player = std::make_unique<SpriteAnimationPlayer>();
		if (!player->create(*mResources))
		{
			LogError("SpriteAnimationPlayer::create() failed");
			return nullptr;
		}

		player->setTitle(title);
		player->setSprite(sprite);

		player->onResize.subscribe([this](auto width, auto height) {
			onPlayerResize(width, height);
		});

		auto* resolution = player->getSelectedResolution();
		player->resize(resolution->width, resolution->height);

		auto* playerPtr = player.get();
		mWidgets.push_back(std::move(player));

		return playerPtr;
	}

	void SpriteEditorApp::onMainMenuClick(const std::string& name)
	{
		EditorApp::onMainMenuClick(name);

		if (name == "openSprite")
		{
			if (mFileDialog != nullptr)
			{
				mFileDialog->show(AssetFileDialogType::Open, "Open Sprite");
			}
		}
		else if (name == "saveSprite")
		{
			if (mFileDialog != nullptr)
			{
				mFileDialog->show(AssetFileDialogType::Save, "Save Sprite");
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

	void SpriteEditorApp::onAssetFileDialogClick(AssetFileDialogType dialogType, AssetFileDialogResult result, 
		const std::string& path)
	{
		if (result == AssetFileDialogResult::Cancel)
		{
			return;
		}

		if (dialogType == AssetFileDialogType::Open)
		{
			auto* sprite = openSprite(path);
			if (sprite != nullptr)
			{
				if (mCurrentSprite != nullptr)
				{
					mResourceCache->removeResource(mCurrentSprite);
				}

				mCurrentSprite = sprite;
				mHierarchy->setSprite(*mCurrentSprite);
				mViewport->setSprite(*mCurrentSprite);
				mInspector->setSprite(*mCurrentSprite);
				mAnimPlayer->setSprite(*mCurrentSprite);
				mViewport->setTitle(mCurrentSprite->getName());

				onSelectFrameHierarchyClick(0);
				onSelectAnimationClick(0);
			}
			else
			{
				mMessageBox->show(std::format("Unable to open sprite file '{}'", path), "Error",
					MessageBoxButtons::Ok, MessageBoxIcon::Error);
			}
		}
		else if (dialogType == AssetFileDialogType::Save || dialogType == AssetFileDialogType::SaveAs)
		{
			if (!saveSprite(mCurrentSprite, path))
			{
				mMessageBox->show(std::format("Unable to save sprite file '{}'", path), "Error",
					MessageBoxButtons::Ok, MessageBoxIcon::Error);
			}

			if (mAssetBrowser != nullptr)
			{
				auto& fileSystem = FileSystem::get();
				mAssetBrowser->refreshPath(fileSystem.getDirectory(path));
			}
		}
	}

	void SpriteEditorApp::onSelectFrameHierarchyClick(uint32_t selectedFrame)
	{
		if (mInspector != nullptr)
		{
			mInspector->setSelectedFrame(selectedFrame);
		}

		if (mViewport != nullptr)
		{
			mViewport->setSelectedFrame(selectedFrame);
		}
	}

	void SpriteEditorApp::onSelectFrameViewportClick(uint32_t selectedFrame)
	{
		if (mInspector != nullptr)
		{
			mInspector->setSelectedFrame(selectedFrame);
		}

		if (mHierarchy != nullptr)
		{
			mHierarchy->setSelectedFrame(selectedFrame);
		}
	}

	void SpriteEditorApp::onSelectAnimationClick(uint32_t selectedAnimation)
	{
		if (mInspector != nullptr)
		{
			mInspector->setSelectedAnimation(selectedAnimation);
		}

		if (mAnimPlayer != nullptr)
		{
			auto* animation = mCurrentSprite->getAnimation(selectedAnimation);
			if (animation != nullptr)
			{
				mAnimPlayer->setAnimation(animation->name);
			}
		}
	}

	void SpriteEditorApp::onViewportResize(uint32_t width, uint32_t height)
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

	void SpriteEditorApp::onPlayerResize(uint32_t width, uint32_t height)
	{
		if (mAnimPlayer != nullptr)
		{
			auto* frameBuffer = mAnimPlayer->getFrameBuffer();
			auto* colorTarget = frameBuffer->getColorTexture();

			if (colorTarget != nullptr)
			{
				mImGuiRenderer->invalidateTexture(*colorTarget);
			}
		}
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