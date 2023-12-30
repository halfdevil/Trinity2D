#include "ParticleEditorApp.h"
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
#include "Widgets/ParticleHierarchy.h"
#include "Widgets/ParticleInspector.h"
#include "Widgets/ParticleViewport.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiFont.h"
#include "Scene/Scene.h"
#include "Scene/SceneSystem.h"
#include "Scene/Components/Camera.h"
#include "Scene/ComponentFactory.h"
#include "Particle/ParticleEffect.h"
#include "Particle/ParticleEmitter.h"
#include "Particle/ParticlePool.h"
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
	bool ParticleEditorApp::init()
	{
		if (!EditorApp::init())
		{
			return false;
		}

		mCurrentParticle = createDefaultParticle();
		if (!mCurrentParticle)
		{
			LogError("ParticleEditorApp::createDefaultParticle() failed");
			return false;
		}

		mHierarchy = createHierarchy("Emitters", *mCurrentParticle);
		mInspector = createInspector("Inspector", *mCurrentParticle);
		mViewport = createViewport(mCurrentParticle->getName(), *mCurrentParticle);

		return true;
	}

	void ParticleEditorApp::setupInput()
	{
		EditorApp::setupInput();
	}

	void ParticleEditorApp::onUpdate(float deltaTime)
	{
		if (mViewport != nullptr)
		{
			mViewport->update(deltaTime);
		}
	}

	void ParticleEditorApp::onDraw(float deltaTime)
	{
		EditorApp::onDraw(deltaTime);

		if (mViewport != nullptr)
		{
			mViewport->drawContent(deltaTime);
		}
	}

	ParticleEffect* ParticleEditorApp::openParticle(const std::string& path)
	{
		auto& fileSystem = FileSystem::get();

		auto particle = std::make_unique<ParticleEffect>();
		particle->setName(fileSystem.getFileName(path, false));
		particle->setFileName(path);
		particle->init(glm::vec2{ 0.0f }, 0.0f);

		auto jsonStr = fileSystem.readText(path);
		if (jsonStr.empty())
		{
			LogError("FileSystem::readText() failed for: '%s'", path.c_str());
			return nullptr;
		}

		auto* serializer = particle->getSerializer();
		json particleJson = json::parse(jsonStr);

		if (!serializer->read(particleJson, *mResourceCache))
		{
			LogError("ParticleSerializer::read() from json failed for particle with path: '%s'", path.c_str());
			return nullptr;
		}

		auto* particlePtr = particle.get();
		mResourceCache->addResource(std::move(particle));

		return particlePtr;
	}

	bool ParticleEditorApp::saveParticle(ParticleEffect* particle, const std::string& path)
	{
		if (!particle)
		{
			LogError("Particle is null for path: '%s'", path.c_str());
			return false;
		}

		auto* serializer = particle->getSerializer();
		json particleJson;

		if (!serializer->write(particleJson))
		{
			LogError("ParticleSerializer::write() to json failed for particle with path: '%s'", path.c_str());
			return false;
		}

		auto& fileSystem = FileSystem::get();
		if (!fileSystem.writeText(path, particleJson.dump(4)))
		{
			LogError("FileSystem::writeText() for particle with path: '%s'", path.c_str());
			return false;
		}

		return true;
	}

	MenuBar* ParticleEditorApp::createMainMenu()
	{
		auto* mainMenu = EditorApp::createMainMenu();
		auto* fileMenu = mainMenu->addMenuItem("file", "File");
		auto* editMenu = mainMenu->addMenuItem("edit", "Edit");
		auto* viewMenu = mainMenu->addMenuItem("view", "View");

		mainMenu->addMenuItem("newParticle", "  New Particle  ", "CTRL+N", fileMenu);
		mainMenu->addMenuItem("openParticle", "  Open Particle  ", "CTRL+O", fileMenu);
		mainMenu->addSeparator(fileMenu);
		mainMenu->addMenuItem("saveParticle", "  Save Particle  ", "CTRL+S", fileMenu);
		mainMenu->addMenuItem("saveAsParticle", "  Save Particle As  ", "CTRL+SHIFT+S", fileMenu);
		mainMenu->addSeparator(fileMenu);
		mainMenu->addMenuItem("exit", "  Exit  ", "ALT+F4", fileMenu);

		mainMenu->addMenuItem("cut", "  Cut  ", "CTRL+X", editMenu);
		mainMenu->addMenuItem("copy", "  Copy  ", "CTRL+C", editMenu);
		mainMenu->addMenuItem("paste", "  Paste  ", "CTRL+V", editMenu);
		mainMenu->addMenuItem("inspector", "  Inspector  ", "CTRL+I", viewMenu);

		return mainMenu;
	}

	AssetFileDialog* ParticleEditorApp::createFileDialog()
	{
		auto* fileDialog = EditorApp::createFileDialog();
		fileDialog->addFileType("Particle File (*.tpar)", { ".tpar" });

		return fileDialog;
	}

	ParticleEffect* ParticleEditorApp::createDefaultParticle() const
	{
		auto particle = std::make_unique<ParticleEffect>();
		particle->setName("New Particle");
		particle->init(glm::vec2{ 0.0f }, 0.0f);

		auto* particlePtr = particle.get();
		mResourceCache->addResource(std::move(particle));

		return particlePtr;
	}

	ParticleHierarchy* ParticleEditorApp::createHierarchy(const std::string& title, ParticleEffect& sprite)
	{
		auto hierarchy = std::make_unique<ParticleHierarchy>();
		hierarchy->setTitle(title);
		hierarchy->setParticle(sprite);

		hierarchy->onSelectEmitter.subscribe([this](auto selectedEmitter) {
			onSelectEmitterClick(selectedEmitter);
		});

		auto* hierarchyPtr = hierarchy.get();
		mWidgets.push_back(std::move(hierarchy));

		return hierarchyPtr;
	}

	ParticleInspector* ParticleEditorApp::createInspector(const std::string& title, ParticleEffect& sprite)
	{
		auto inspector = std::make_unique<ParticleInspector>();
		inspector->setTitle(title);
		inspector->setLayout(*mLayout);
		inspector->setResourceCache(*mResourceCache);
		inspector->setParticle(*mCurrentParticle);

		auto* inspectorPtr = inspector.get();
		mWidgets.push_back(std::move(inspector));

		return inspectorPtr;
	}

	ParticleViewport* ParticleEditorApp::createViewport(const std::string& title, ParticleEffect& sprite)
	{
		auto viewport = std::make_unique<ParticleViewport>();
		if (!viewport->create(*mResources))
		{
			LogError("ParticleViewport::create() failed");
			return nullptr;
		}

		viewport->setTitle(title);
		viewport->setParticle(sprite);

		viewport->onResize.subscribe([this](auto width, auto height) {
			onViewportResize(width, height);
		});

		auto* resolution = viewport->getSelectedResolution();
		viewport->resize(resolution->width, resolution->height);

		auto* viewportPtr = viewport.get();
		mWidgets.push_back(std::move(viewport));

		return viewportPtr;
	}

	void ParticleEditorApp::onMainMenuClick(const std::string& name)
	{
		EditorApp::onMainMenuClick(name);

		if (name == "openParticle")
		{
			if (mFileDialog != nullptr)
			{
				mFileDialog->show(AssetFileDialogType::Open, "Open Particle");
			}
		}
		else if (name == "saveParticle")
		{
			if (mFileDialog != nullptr)
			{
				mFileDialog->show(AssetFileDialogType::Save, "Save Particle");
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

	void ParticleEditorApp::onAssetFileDialogClick(AssetFileDialogType dialogType, AssetFileDialogResult result, 
		const std::string& path)
	{
		if (result == AssetFileDialogResult::Cancel)
		{
			return;
		}

		if (dialogType == AssetFileDialogType::Open)
		{
			auto* sprite = openParticle(path);
			if (sprite != nullptr)
			{
				if (mCurrentParticle != nullptr)
				{
					mResourceCache->removeResource(mCurrentParticle);
				}

				mCurrentParticle = sprite;
				mHierarchy->setParticle(*mCurrentParticle);
				mViewport->setParticle(*mCurrentParticle);
				mInspector->setParticle(*mCurrentParticle);
				onSelectEmitterClick(0);
			}
			else
			{
				mMessageBox->show(std::format("Unable to open particle file '{}'", path), "Error",
					MessageBoxButtons::Ok, MessageBoxIcon::Error);
			}
		}
		else if (dialogType == AssetFileDialogType::Save || dialogType == AssetFileDialogType::SaveAs)
		{
			if (!saveParticle(mCurrentParticle, path))
			{
				mMessageBox->show(std::format("Unable to save particle file '{}'", path), "Error",
					MessageBoxButtons::Ok, MessageBoxIcon::Error);
			}

			if (mAssetBrowser != nullptr)
			{
				auto& fileSystem = FileSystem::get();
				mAssetBrowser->refreshPath(fileSystem.getDirectory(path));
			}
		}
	}

	void ParticleEditorApp::onSelectEmitterClick(uint32_t selectedEmitter)
	{
		if (mInspector != nullptr)
		{
			mInspector->setSelectedEmitter(selectedEmitter);
		}
	}

	void ParticleEditorApp::onViewportResize(uint32_t width, uint32_t height)
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
}

using namespace Trinity;

int main(int argc, char* argv[])
{
	static ParticleEditorApp app;
	app.run({
		.logLevel = LogLevel::Info,
		.title = "Trinity2D - Particle Editor",
#ifdef __EMSCRIPTEN__
		.configFile = "/Assets/EditorConfig.json",
#else
		.configFile = "EditorConfig.json",
#endif
	});

	return 0;
}