#include "Playground.h"
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
	bool Playground::init()
	{
		if (!Application::init())
		{
			return false;
		}

		auto& swapChain = mGraphicsDevice->getSwapChain();
		mImGuiRenderer = std::make_unique<ImGuiRenderer>();

		if (!mImGuiRenderer->create(*mWindow, "/Assets/Fonts/CascadiaCode.ttf", swapChain))
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

		mFont = font.get();
		mScene = scene.get();
		mCamera = camera;

		mResourceCache->addResource(std::move(font));
		mResourceCache->addResource(std::move(scene));

		return true;
	}

	void Playground::draw(float deltaTime)
	{
		Application::draw(deltaTime);
		onGui();
	}

	void Playground::setupInput()
	{
		Application::setupInput();

		mInput->bindAction("exit", InputEvent::Pressed, [this](int32_t key) {
			exit();
		});
	}

	void Playground::onGui()
	{
		auto projView = mCamera->getProjection() * mCamera->getView();
		auto transform = glm::translate(glm::mat4(1.0f), glm::vec3{ 200.0f, 200.0f, 1.0f });

		mTextRenderer->begin(projView);
		mTextRenderer->drawText("Hello World!", mFont, 64.0f, glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f }, glm::vec2{ 0.0f }, transform);
		mTextRenderer->end();
	}
}

using namespace Trinity;

int main(int argc, char* argv[])
{
	static Playground app;
	app.run({
		.title = "Trinity2D - Playground",
#ifdef __EMSCRIPTEN__
		.configFile = "/Assets/AppConfig.json",
#else
		.configFile = "AppConfig.json",
#endif
	});

	return 0;
}