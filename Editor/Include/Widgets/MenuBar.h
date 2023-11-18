#pragma once

#include "Widgets/Menu.h"

namespace Trinity
{
	class MenuBar : public Menu
	{
	public:

		MenuBar() = default;
		virtual ~MenuBar() = default;

		MenuBar(const MenuBar&) = delete;
		MenuBar& operator = (const MenuBar&) = delete;

		MenuBar(MenuBar&&) = default;
		MenuBar& operator = (MenuBar&&) = default;

		virtual void draw() override;
	};
}