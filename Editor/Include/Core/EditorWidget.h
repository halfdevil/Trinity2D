#pragma once

#include "imgui.h"

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

		virtual bool isEnabled() const;
		virtual void setEnabled(bool enabled);
		virtual void draw() = 0;

	protected:

		bool mEnabled{ true };
	};
}