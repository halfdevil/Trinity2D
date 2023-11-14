#pragma once

#include "EditorWidget.h"

namespace Trinity
{
	class SpriteEditor : public EditorWidget
	{
	public:

		SpriteEditor() = default;
		virtual ~SpriteEditor() = default;

		SpriteEditor(const SpriteEditor&) = delete;
		SpriteEditor& operator = (const SpriteEditor&) = delete;

		SpriteEditor(SpriteEditor&&) = default;
		SpriteEditor& operator = (SpriteEditor&&) = default;
	};
}