#include "Widgets/EditorMenu.h"
#include "ImGui/ImGuiRenderer.h"
#include <stack>

namespace Trinity
{
	EditorMenuItem* EditorMenu::addMenuItem(const std::string& name, const std::string& title, 
		const std::string& shortcut, EditorMenuItem* parent)
	{
		auto menuItem = std::make_unique<EditorMenuItem>(name, title, shortcut);
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

	EditorMenuItem* EditorMenu::findMenuItem(const std::string& title)
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

	void EditorMenu::setMainMenu(bool mainMenu)
	{
		mMainMenu = mainMenu;
	}

	bool EditorMenu::isEnabled() const
	{
		return mEnabled && mAllItems.size() > 0;
	}

	void EditorMenu::draw()
	{
		if (isEnabled())
		{
			if (mMainMenu ? ImGui::BeginMainMenuBar() : true)
			{
				for (auto* menuItem : mTopItems)
				{
					drawMenuItem(menuItem);
				}

				if (mMainMenu)
				{
					ImGui::EndMainMenuBar();
				}
			}
		}
	}

	void EditorMenu::drawMenuItem(EditorMenuItem* menuItem)
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