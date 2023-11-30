#include "Widgets/Inspector.h"
#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "Scene/ComponentFactory.h"
#include "Editor/Editor.h"
#include "Core/EditorLayout.h"
#include "Core/EditorTheme.h"
#include "misc/cpp/imgui_stdlib.h"
#include "IconsFontAwesome6.h"

namespace Trinity
{
	void Inspector::setLayout(EditorLayout& layout)
	{
		mLayout = &layout;
	}

	void Inspector::setResourceCache(ResourceCache& cache)
	{
		mResourceCache = &cache;
	}

	void Inspector::draw()
	{
		if (!isEnabled())
		{
			return;
		}

		ImGui::Begin(mTitle.c_str(), &mEnabled);
		{
			drawInspector();
			ImGui::End();
		}
	}

	void Inspector::drawInspector()
	{
	}
}