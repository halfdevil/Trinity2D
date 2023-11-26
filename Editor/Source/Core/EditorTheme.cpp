#include "Core/EditorTheme.h"
#include "Core/Application.h"
#include "Core/Window.h"

namespace Trinity
{	
	EditorTheme::~EditorTheme()
	{
		destroy();
	}

	bool EditorTheme::create(const std::string& fileName)
	{
		auto& application = Application::get();
		auto* window = application.getWindow();
		auto scaleFactor = window->getScaleFactor();

		buttonSize = { kBaseButtonSize.x * scaleFactor, kBaseButtonSize.y * scaleFactor };
		return true;
	}

	void EditorTheme::destroy()
	{
	}
}