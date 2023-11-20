#include "PlaygroundApp.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiFont.h"
#include "Gui/Font.h"
#include "Gui/TextRenderer.h"
#include "Scene/Scene.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/Scripts/CameraController.h"
#include "Scene/ComponentFactory.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderPass.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/FrameBuffer.h"
#include "VFS/FileSystem.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "Core/Debugger.h"
#include "Core/Clock.h"
#include "Core/Window.h"

namespace Trinity
{
	bool PlaygroundApp::init()
	{
		if (!Application::init())
		{
			return false;
		}

		auto& swapChain = mGraphicsDevice->getSwapChain();
		mRenderPass = std::make_unique<RenderPass>();		

		auto frameBuffer = std::make_unique<FrameBuffer>();
		if (!frameBuffer->create(512, 512))
		{
			LogError("FrameBuffer::create() failed");
			return false;
		}

		if (!frameBuffer->addColorAttachment(wgpu::TextureFormat::BGRA8Unorm, wgpu::TextureUsage::RenderAttachment |
			wgpu::TextureUsage::TextureBinding))
		{
			LogError("FrameBuffer::addColorAttachment() failed");
			return false;
		}

		if (!frameBuffer->setDepthStencilAttachment(wgpu::TextureFormat::Depth32Float, wgpu::TextureUsage::RenderAttachment))
		{
			LogError("FrameBuffer::setDepthStencilAttachment() failed");
			return false;
		}

		mImGuiRenderer = std::make_unique<ImGuiRenderer>();
		if (!mImGuiRenderer->create(*mWindow, swapChain))
		{
			LogError("Gui::create() failed!!");
			return false;
		}

		mTextRenderer = std::make_unique<TextRenderer>();
		if (!mTextRenderer->create(TextRenderer::kDefaultShader, swapChain))
		{
			LogError("TextRenderer::create() failed");
			return false;
		}

		mBatchRenderer = std::make_unique<BatchRenderer>();
		if (!mBatchRenderer->create("/Assets/Engine/Shaders/SpriteRenderer.wgsl", swapChain))
		{
			LogError("BatchRenderer::create() failed");
			return false;
		}

		auto imGuiFont = std::make_unique<ImGuiFont>();
		if (!imGuiFont->create("default", "/Assets/Fonts/CascadiaCode.ttf"))
		{
			LogError("ImGuiFont::create() failed");
			return false;
		}

		if (!imGuiFont->build())
		{
			LogError("ImGuiFont::build() failed");
			return false;
		}

		auto texture = std::make_unique<Texture>();
		if (!texture->create("/Assets/Textures/grass.png", wgpu::TextureFormat::RGBA8Unorm))
		{
			LogError("Texture::create() failed");
			return false;
		}

		auto font = std::make_unique<Font>();
		if (!font->create("/Assets/Fonts/CascadiaCode.ttf", { 16.0f, 32.0f, 64.0f }))
		{
			LogError("Font::create() failed for: 'CascadiaCode.ttf'");
			return false;
		}

		auto scene = std::make_unique<Scene>();
		auto rootNode = std::make_unique<Node>();
		rootNode->setName("root");

		scene->setRoot(*rootNode);
		scene->addNode(std::move(rootNode));

		auto* camera = scene->addCamera(
			"mainCamera", 
			{ (float)mWindow->getWidth(), (float)mWindow->getHeight() },
			0.1f, 
			100.0f
		);

		auto* cameraController = scene->addCameraController("mainCamera");
		cameraController->setMoveSpeed(0.1f);
		cameraController->setRotationSpeed(0.001f);

		mFrameBuffer = frameBuffer.get();
		mTexture = texture.get();
		mFont = font.get();
		mScene = scene.get();
		mCamera = camera;
		mCameraController = cameraController;

		mResourceCache->addResource(std::move(imGuiFont));
		mResourceCache->addResource(std::move(frameBuffer));
		mResourceCache->addResource(std::move(texture));
		mResourceCache->addResource(std::move(font));
		mResourceCache->addResource(std::move(scene));

		mScripts = mScene->getComponents<Script>();
		for (auto& script : mScripts)
		{
			script->init();
		}

		return true;
	}

	void PlaygroundApp::update(float deltaTime)
	{
		Application::update(deltaTime);

		if (mInput->getMouse()->isButtonDown(MOUSE_BUTTON_1))
		{
			if (mCameraController != nullptr)
			{
				auto relPos = mInput->getMouse()->getRelativePosition();
				auto delta = glm::vec2{ -(float)relPos.x, -(float)relPos.y };

				LogError("Delta: { %f, %f }", delta.x, delta.y);

				mCameraController->panBy(delta);
			}
		}

		if (mScene != nullptr)
		{
			for (auto& script : mScripts)
			{
				script->update(deltaTime);
			}
		}
	}

	void PlaygroundApp::draw(float deltaTime)
	{
		Application::draw(deltaTime);
		onGui();
	}

	void PlaygroundApp::setupInput()
	{
		Application::setupInput();

		mInput->bindAction("exit", InputEvent::Pressed, [this](int32_t key) {
			exit();
		});

		mInput->bindAxis("moveRight", [this](float scale) {
			moveRight(scale);
		}); 
		
		mInput->bindAxis("moveUp", [this](float scale) {
			moveUp(scale);
		});

		mInput->bindAxis("turn", [this](float scale) {
			turn(scale);
		});
	}

	void PlaygroundApp::onGui()
	{
		auto projView = mCamera->getProjection() * mCamera->getView();
		auto transform = glm::translate(glm::mat4(1.0f), glm::vec3{ -256.0f, -256.0f, 1.0f });

		auto& swapChain = mGraphicsDevice->getSwapChain();
		mRenderPass->begin(swapChain);
		
		mBatchRenderer->begin(projView);
		mBatchRenderer->drawTexture(mTexture, glm::vec2{ 0.0f }, glm::vec2{ 512.0f }, glm::vec2{ 0.5f }, transform);
		mBatchRenderer->end(*mRenderPass);

		mRenderPass->end();
	}

	void PlaygroundApp::moveRight(float scale)
	{
		if (mCameraController != nullptr && scale != 0.0f)
		{
			mCameraController->moveRight(scale);
		}
	}

	void PlaygroundApp::moveUp(float scale)
	{
		if (mCameraController != nullptr && scale != 0.0f)
		{
			mCameraController->moveUp(scale);
		}
	}

	void PlaygroundApp::turn(float scale)
	{
		if (mCameraController != nullptr && scale != 0.0f)
		{
			mCameraController->turn(scale);
		}
	}
}

using namespace Trinity;

int main(int argc, char* argv[])
{
	static PlaygroundApp app;
	app.run({
		.title = "Trinity2D - PlaygroundApp",
#ifdef __EMSCRIPTEN__
		.configFile = "/Assets/AppConfig.json",
#else
		.configFile = "AppConfig.json",
#endif
	});

	return 0;
}