#include "Widgets/Menu.h"
#include "ImGui/ImGuiRenderer.h"
#include <stack>

namespace Trinity
{
	MenuItem* Menu::addMenuItem(const std::string& name, const std::string& title, 
		const std::string& shortcut, MenuItem* parent)
	{
		auto menuItem = std::make_unique<MenuItem>(name, title, shortcut);
		if (parent != nullptr)
		{
			parent->children.push_back(menuItem.get());
		}
		else
		{
			mTopItems.push_back(menuItem.get());
		}

		auto* menuItemPtr = menuItem.get();
		mAllItems.push_back(std::move(menuItem));

		return menuItemPtr;
	}

	MenuItem* Menu::findMenuItem(const std::string& title)
	{
		for (auto& menuItem : mAllItems)
		{
			if (menuItem->title == title)
			{
				return menuItem.get();
			}
		}

		return nullptr;
	}

	bool Menu::isEnabled() const
	{
		return mEnabled && mAllItems.size() > 0;
	}

	void Menu::draw()
	{
		if (isEnabled())
		{
			for (auto* menuItem : mTopItems)
			{
				drawMenuItem(menuItem);
			}
		}
	}

	void Menu::drawMenuItem(MenuItem* menuItem)
	{
		auto& children = menuItem->children;
		if (children.size() > 0)
		{
			if (ImGui::BeginMenu(menuItem->title.c_str()))
			{
				for (auto* childItem : children)
				{
					drawMenuItem(childItem);
				}

				ImGui::EndMenu();
			}
		}
		else
		{
			if (ImGui::MenuItem(menuItem->title.c_str(), menuItem->shortcut.c_str()))
			{
				onMenuItemClick.notify(*menuItem);
			}
		}
	}
}