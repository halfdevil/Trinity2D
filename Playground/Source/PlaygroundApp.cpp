#include "PlaygroundApp.h"
#include "Input/Input.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiFont.h"
#include "Gui/Font.h"
#include "Gui/TextRenderer.h"
#include "Scene/Scene.h"
#include "Scene/Components/Camera.h"
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

		mRenderPass = std::make_unique<RenderPass>();

		auto colorTexture = std::make_unique<Texture>();
		if (!colorTexture->create(512, 512, wgpu::TextureFormat::RGBA8Unorm, wgpu::TextureUsage::RenderAttachment |
			wgpu::TextureUsage::TextureBinding))
		{
			LogError("Texture::create() failed");
			return false;
		}

		auto depthTexture = std::make_unique<Texture>();
		if (!depthTexture->create(512, 512, wgpu::TextureFormat::Depth32Float, wgpu::TextureUsage::RenderAttachment))
		{
			LogError("Texture::create() failed");
			return false;
		}

		auto frameBuffer = std::make_unique<FrameBuffer>();
		frameBuffer->addColorAttachment(*colorTexture);
		frameBuffer->setDepthStencilAttachment(*depthTexture);

		auto& swapChain = mGraphicsDevice->getSwapChain();
		mImGuiRenderer = std::make_unique<ImGuiRenderer>();

		if (!mImGuiRenderer->create(*mWindow, swapChain))
		{
			LogError("Gui::create() failed!!");
			return false;
		}

		mTextRenderer = std::make_unique<TextRenderer>();
		if (!mTextRenderer->create(TextRenderer::kDefaultShader, *frameBuffer))
		{
			LogError("TextRenderer::create() failed");
			return false;
		}

		mBatchRenderer = std::make_unique<BatchRenderer>();
		if (!mBatchRenderer->create("/Assets/Engine/Shaders/SpriteRenderer.wgsl", *frameBuffer))
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
		if (!scene->create("newScene"))
		{
			LogError("Scene::create() failed");
			return false;
		}

		auto rootNode = std::make_unique<Node>();
		rootNode->setName("root");

		scene->setRoot(*rootNode);
		scene->addNode(std::move(rootNode));

		auto* camera = scene->addCamera("mainCamera", 0.0f, (float)mWindow->getWidth(), (float)mWindow->getHeight(), 
			0.0f, 0.1f, 100.0f, glm::vec3(0.0f));

		mColorTexture = colorTexture.get();
		mDepthTexture = depthTexture.get();
		mFrameBuffer = frameBuffer.get();
		mTexture = texture.get();
		mFont = font.get();
		mScene = scene.get();
		mCamera = camera;

		mResourceCache->addResource(std::move(imGuiFont));
		mResourceCache->addResource(std::move(colorTexture));
		mResourceCache->addResource(std::move(depthTexture));
		mResourceCache->addResource(std::move(frameBuffer));
		mResourceCache->addResource(std::move(texture));
		mResourceCache->addResource(std::move(font));
		mResourceCache->addResource(std::move(scene));

		return true;
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
	}

	void PlaygroundApp::onGui()
	{
		auto projView = mCamera->getProjection() * mCamera->getView();
		auto transform = glm::translate(glm::mat4(1.0f), glm::vec3{ 200.0f, 200.0f, 1.0f });

		mRenderPass->begin(*mFrameBuffer);
		
		mBatchRenderer->begin(projView);
		mBatchRenderer->drawTexture(mTexture, glm::vec2{ 0.0f }, glm::vec2{ 512.0f }, 
			glm::vec2{ 0.0f }, glm::vec2{ 512.0f }, glm::vec4{ 0.0f }, 1.0f);
		mBatchRenderer->end(*mRenderPass);

		mTextRenderer->begin(projView);
		mTextRenderer->drawText("Hello World!", mFont, 64.0f, glm::vec4{ 1.0f }, 
			glm::vec2{ 0.0f }, transform);
		mTextRenderer->end(*mRenderPass);

		mRenderPass->end();
		
		auto& swapChain = mGraphicsDevice->getSwapChain();
		mRenderPass->begin(swapChain);

		mImGuiRenderer->newFrame(*mWindow, mClock->getDeltaTime());
		ImGui::Begin("Demo Window");
		ImGui::Image((ImTextureID)mColorTexture, ImVec2{ 512, 512 });
		ImGui::End();
		mImGuiRenderer->draw(*mRenderPass);

		mRenderPass->end();
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