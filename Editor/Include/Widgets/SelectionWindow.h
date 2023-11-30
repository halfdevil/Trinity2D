#pragma once

#include "Core/EditorWidget.h"
#include "Core/Observer.h"
#include <string>
#include <memory>
#include <vector>

namespace Trinity
{
	struct SelectionItem
	{
		std::string name;
		std::string title;
	};

	class SelectionWindow : public EditorWidget
	{
	public:

		static constexpr ImVec2 kBaseButtonSize = ImVec2{ 100.0f, 36.0f };

		SelectionWindow() = default;
		virtual ~SelectionWindow() = default;

		SelectionWindow(const SelectionWindow&) = delete;
		SelectionWindow& operator = (const SelectionWindow&) = delete;

		SelectionWindow(SelectionWindow&&) = default;
		SelectionWindow& operator = (SelectionWindow&&) = default;

		virtual void setItems(std::vector<SelectionItem>&& items);
		virtual void setButtonTitle(const std::string& buttonTitle);

		virtual void draw() override;

	public:

		Observer<const SelectionItem&> onSelect;

	protected:

		std::vector<SelectionItem> mItems;
		std::vector<const char*> mItemNames;
		std::string mButtonTitle;
		uint32_t mSelectedIndex{ 0 };
		float mButtonWidth{ 0.0f };
	};
}