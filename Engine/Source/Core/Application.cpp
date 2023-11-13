#include "Core/Application.h"
#include "Core/Logger.h"
#include "Core/Debugger.h"
#include "Core/Clock.h"
#include "Core/Window.h"
#include "Core/ResourceCache.h"
#include "VFS/FileSystem.h"
#include "VFS/DiskFile.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/SwapChain.h"
#include "Graphics/RenderPass.h"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

EM_JS(int, getCanvasWidth, (), {
	return canvas.clientWidth;
});

EM_JS(int, getCanvasHeight, (), {
	return canvas.clientHeight;
});

extern "C"
{
	void EMSCRIPTEN_KEEPALIVE onCanvasResize(int width, int height)
	{
		Trinity::Application::get().getWindow()->setSize(glm::uvec2(width, height));
	}
}

#endif

namespace Trinity
{
	Application::~Application()
	{
	}

	void Application::run(const ApplicationOptions& options)
	{
		mOptions = options;
		mFrameTime = 1.0f / options.fps;
		mMPF = 1000.0f * mFrameTime;

		mLogger = std::make_unique<Logger>();
		mLogger->setMaxLogLevel(options.logLevel);

		mDebugger = std::make_unique<Debugger>();
		mClock = std::make_unique<Clock>();
		mFileSystem = std::make_unique<FileSystem>();
		mInput = std::make_unique<Input>();
		mWindow = std::make_unique<Window>();
		mGraphicsDevice = std::make_unique<GraphicsDevice>();

		if (!Window::initialize())
		{
			LogFatal("Window::initialize() failed!!");
			return;
		}

#ifdef __EMSCRIPTEN__
		mOptions.width = getCanvasWidth();
		mOptions.height = getCanvasHeight();

		LogError("Width: %d, Height: %d", mOptions.width, mOptions.height);
#endif

		if (!mWindow->create(mOptions.title, mOptions.width, mOptions.height, mOptions.displayMode))
		{
			LogFatal("Window::create() failed!!");
			return;
		}

		mGraphicsDevice->onCreated.subscribe([this](bool result) {
			if (result)
			{
				mClock->reset();
				mWindow->show(true);

				if (init())
				{
					setupInput();
#ifdef __EMSCRIPTEN__
					static Application* app = this;
					emscripten_set_main_loop_arg([](void* arg) {
						app->frame();
					}, this, 0, false);
#else
					while (!mWindow->isClosed())
					{
						frame();
						mWindow->poll();
					}
#endif
				}
			}
		});

		mGraphicsDevice->create(*mWindow);
	}

	bool Application::init()
	{
		if (!mFileSystem->addFolder("/Assets", "Assets"))
		{
			LogError("FileSystem::addFolder() failed!!");
			return false;
		}

		if (!mOptions.configFile.empty())
		{
			DiskFile configFile;
			if (!configFile.create(mOptions.configFile, mOptions.configFile, FileOpenMode::OpenRead))
			{
				LogError("PhysicalFile::create() failed for: %s!!", mOptions.configFile.c_str());
				return false;
			}

			FileReader reader(configFile);
			mConfig = json::parse(reader.readAsString());

			if (mConfig.contains("folders"))
			{
				for (auto& folder : mConfig["folders"])
				{
					const std::string folderAlias = folder["alias"].get<std::string>();
					const std::string folderPath = folder["path"].get<std::string>();

					if (!mFileSystem->addFolder(folderAlias, folderPath))
					{
						LogError("FileSystem::addFolder() failed for: %s!!", folderPath.c_str());
						return false;
					}
				}
			}

			if (mConfig.contains("input"))
			{
				if (!mInput->loadConfig(mConfig["input"]))
				{
					LogError("Input::loadConfig() failed!!");
					return false;
				}
			}
		}

		if (!mInput->create(*mWindow))
		{
			LogError("Input::create() failed!!");
			return false;
		}

		WindowCallbacks& callbacks = mWindow->getCallbacks();

		callbacks.onClose.subscribe([this]() {
			onClose();
		});

		callbacks.onResize.subscribe([this](int32_t, int32_t) {
			onResize();
		});

		onResize();

		mGraphicsDevice->setClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });
		mWindow->showMouse(true, false);
		mResourceCache = std::make_unique<ResourceCache>();
		mMainPass = std::make_unique<RenderPass>();

		return true;
	}

	void Application::update(float deltaTime)
	{		
	}

	void Application::fixedUpdate(float deltaTime)
	{
	}

	void Application::draw(float deltaTime)
	{
	}

	void Application::frame()
	{
		mClock->update();
		mInput->update();

		mLagTime += mClock->getDeltaTime();
		while (mLagTime >= mMPF)
		{
			mLagTime -= mMPF;
			fixedUpdate(mMPF);
		}

		mGraphicsDevice->clearScreen();

		update(mClock->getDeltaTime());
		draw(mClock->getDeltaTime());

		mGraphicsDevice->present();
		mInput->postUpdate();
	}

	void Application::exit()
	{
		mWindow->close();
	}

	void Application::onClose()
	{
	}

	void Application::onResize()
	{
		if (!mGraphicsDevice->setupSwapChain(*mWindow))
		{
			LogError("GraphicsDevice::setupSwapChain() failed!!");
		}
	}

	void Application::setupInput()
	{
	}
}