#pragma once

#include "Core/Singleton.h"
#include "glm/glm.hpp"
#include "imgui.h"
#include <string>

namespace Trinity
{
	class EditorTheme : public Singleton<EditorTheme>
	{
	public:

		static constexpr ImVec2 kBaseButtonSize = ImVec2{ 100.0f, 36.0f };

		EditorTheme() = default;
		virtual ~EditorTheme();

		EditorTheme(const EditorTheme&) = delete;
		EditorTheme& operator = (const EditorTheme&) = delete;

		EditorTheme(EditorTheme&&) = default;
		EditorTheme& operator = (EditorTheme&&) = default;

		float getScaleFactor() const
		{
			return mScaleFactor;
		}

		virtual bool create(const std::string& fileName);
		virtual void destroy();

	public:

		ImVec2 buttonSize{ 0.0f, 0.0f };
		float mScaleFactor{ 1.0f };
	};
}