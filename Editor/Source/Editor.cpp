#include "Editor.h"
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
	bool Editor::init()
	{
		if (!Application::init())
		{
			return false;
		}

		mImGuiRenderer = std::make_unique<ImGuiRenderer>();
		if (!mImGuiRenderer->create(*mWindow, "/Assets/Fonts/CascadiaCode.ttf"))
		{
			LogError("Gui::create() failed!!");
			return false;
		}

		mTextRenderer = std::make_unique<TextRenderer>();
		if (!mTextRenderer->create())
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

		mFont = font.get();
		mResourceCache->addResource(std::move(font));

		return true;
	}

	void Editor::draw(float deltaTime)
	{
		Application::draw(deltaTime);
		onGui();
	}

	void Editor::setupInput()
	{
		Application::setupInput();

		mInput->bindAction("exit", InputEvent::Pressed, [this](int32_t key) {
			exit();
		});
	}

	void Editor::onGui()
	{
	}
}

using namespace Trinity;

int main(int argc, char* argv[])
{
	static Editor app;
	app.run({
		.title = "Trinity2D - Editor",
#ifdef __EMSCRIPTEN__
		.configFile = "/Assets/EditorConfig.json",
#else
		.configFile = "EditorConfig.json",
#endif
	});

	return 0;
}