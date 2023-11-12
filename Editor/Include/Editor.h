#pragma once

#include "Core/Application.h"

namespace Trinity
{
	class Font;
	class ImGuiRenderer;
	class TextRenderer;

	class Editor : public Application
	{
	public:

		Editor() = default;
		virtual ~Editor() = default;

		Editor(const Editor&) = delete;
		Editor& operator = (const Editor&) = delete;

		Editor(Editor&&) = default;
		Editor& operator = (Editor&&) = default;

	protected:

		virtual bool init() override;
		virtual void draw(float deltaTime) override;
		virtual void setupInput() override;
		virtual void onGui();

	protected:

		std::unique_ptr<ImGuiRenderer> mImGuiRenderer{ nullptr };
		std::unique_ptr<TextRenderer> mTextRenderer{ nullptr };
		Font* mFont{ nullptr };
	};
}