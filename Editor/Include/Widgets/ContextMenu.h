#pragma once

#include "Widgets/Menu.h"

namespace Trinity
{
	class ContextMenu : public Menu
	{
	public:

		ContextMenu() = default;
		virtual ~ContextMenu() = default;

		ContextMenu(const ContextMenu&) = delete;
		ContextMenu& operator = (const ContextMenu&) = delete;

		ContextMenu(ContextMenu&&) = default;
		ContextMenu& operator = (ContextMenu&&) = default;

		bool isOpen() const
		{
			return mIsOpened;
		}

		virtual void draw() override;

	protected:

		bool mIsOpened{ false };
	};
}