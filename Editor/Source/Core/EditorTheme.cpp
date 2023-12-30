#include "Core/EditorTheme.h"
#include "Core/Application.h"
#include "Core/Window.h"

namespace Trinity
{	
	void EditorTheme::init(const std::string& fileName)
	{
		auto& application = Application::get();
		auto* window = application.getWindow();

		mScaleFactor = window->getScaleFactor();
		buttonSize = { kBaseButtonSize.x * mScaleFactor, kBaseButtonSize.y * mScaleFactor };
	}
}