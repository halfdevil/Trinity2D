#pragma once

namespace Trinity
{
	class EditorLayout;

	class Editor
	{
	public:

		virtual void onInspectorGui(const EditorLayout& layout) = 0;
	};
}