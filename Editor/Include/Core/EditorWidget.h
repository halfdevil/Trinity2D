#pragma once

#include "imgui.h"
#include <string>

namespace Trinity
{
	class EditorWidget
	{
	public:

		EditorWidget() = default;
		virtual ~EditorWidget() = default;

		EditorWidget(const EditorWidget&) = delete;
		EditorWidget& operator = (const EditorWidget&) = delete;

		EditorWidget(EditorWidget&&) = default;
		EditorWidget& operator = (EditorWidget&&) = default;

		const std::string& getTitle() const
		{
			return mTitle;
		}

		virtual bool isEnabled() const;
		virtual void setEnabled(bool enabled);
		virtual void setTitle(const std::string& title);
		virtual void draw() = 0;

	protected:

		bool mEnabled{ true };
		std::string mTitle;
	};
}