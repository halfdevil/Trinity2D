#pragma once

#include "Core/EditorWidget.h"
#include "Core/Observer.h"
#include <string>
#include <vector>
#include <memory>

namespace Trinity
{
	struct MenuItem
	{
		std::string name;
		std::string title;
		std::string shortcut;
		std::vector<MenuItem*> children;

		MenuItem(const std::string& inName, const std::string& inTitle, const std::string& inShortcut) :
			name(inName),
			title(inTitle),
			shortcut(inShortcut)
		{
		}
	};

	class Menu : public EditorWidget
	{
	public:

		Menu() = default;
		virtual ~Menu() = default;

		Menu(const Menu&) = delete;
		Menu& operator = (const Menu&) = delete;

		Menu(Menu&&) = default;
		Menu& operator = (Menu&&) = default;

		virtual MenuItem* addMenuItem(
			const std::string& name,
			const std::string& title, 
			const std::string& shortcut = "",
			MenuItem* parent = nullptr
		);		

		virtual void addSeparator(MenuItem* parent = nullptr);
		virtual MenuItem* findMenuItem(const std::string& name);

		virtual bool isEnabled() const override;
		virtual void draw() override;

	protected:

		virtual void drawMenuItem(MenuItem* menuItem);

	public:

		Observer<const MenuItem&> onMenuItemClick; 

	protected:

		std::vector<MenuItem*> mTopItems;
		std::vector<std::unique_ptr<MenuItem>> mAllItems;
	};
}