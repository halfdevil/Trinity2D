#pragma once

#include "EditorWidget.h"
#include "Core/Observer.h"
#include <string>
#include <vector>
#include <memory>

namespace Trinity
{
	struct EditorMenuItem
	{
		std::string title;
		std::string shortcut;
		std::vector<EditorMenuItem*> children;

		EditorMenuItem(const std::string& inTitle, const std::string& inShortcut) :
			title(inTitle),
			shortcut(inShortcut)
		{
		}
	};

	class EditorMenu : public EditorWidget
	{
	public:

		EditorMenu() = default;
		virtual ~EditorMenu() = default;

		EditorMenu(const EditorMenu&) = delete;
		EditorMenu& operator = (const EditorMenu&) = delete;

		EditorMenu(EditorMenu&&) = default;
		EditorMenu& operator = (EditorMenu&&) = default;

		bool isMainMenu() const
		{
			return mMainMenu;
		}

		virtual EditorMenuItem* addMenuItem(
			const std::string& title, 
			const std::string& shortcut = "",
			EditorMenuItem* parent = nullptr
		);		

		virtual EditorMenuItem* findMenuItem(const std::string& title);
		virtual void setMainMenu(bool mainMenu);
		virtual bool isEnabled() const override;
		virtual void draw() override;

	protected:

		virtual void drawMenuItem(EditorMenuItem* menuItem);

	public:

		Observer<const EditorMenuItem&> onMenuItemClick; 

	protected:

		std::vector<EditorMenuItem*> mTopItems;
		std::vector<std::unique_ptr<EditorMenuItem>> mAllItems;
		bool mMainMenu{ false };
	};
}