#pragma once

#include "Core/EditorWidget.h"
#include "VFS/Storage.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <deque>

namespace Trinity
{
	class EditorResources;
	class Texture;
	class ImGuiFont;

	struct AssetEntry
	{
		std::string name;
		std::string path;
		bool isFolder{ false };
		bool isOpen{ false };
		bool hasFolders{ false };
		std::vector<FileEntry> fileEntries;

		AssetEntry(
			const std::string& inName, 
			const std::string& inPath, 
			bool inIsFolder = false,
			bool inHasFolders = false,
			std::vector<FileEntry>&& inEntries = {}
		) :
			name(inName),
			path(inPath),
			isFolder(inIsFolder),
			hasFolders(inHasFolders),
			fileEntries(std::move(inEntries))
		{
		}
	};

	class AssetBrowser : public EditorWidget
	{
	public:

		static constexpr uint32_t kMaxQueueSize = 50;

		AssetBrowser() = default;
		virtual ~AssetBrowser();

		AssetBrowser(const AssetBrowser&) = delete;
		AssetBrowser& operator = (const AssetBrowser&) = delete;

		AssetBrowser(AssetBrowser&&) = default;
		AssetBrowser& operator = (AssetBrowser&&) = default;

		const std::string& getTitle() const
		{
			return mTitle;
		}

		const std::string& getRootFolder() const
		{
			return mRootFolder;
		}

		virtual bool create(const std::string& rootFolder, EditorResources& resources);
		virtual void destroy();

		virtual void setTitle(const std::string& title);
		virtual void draw() override;

	protected:

		virtual AssetEntry* updateEntry(const std::string& folderPath);
		virtual AssetEntry* getEntry(const std::string& folderPath);

		virtual void drawTopView(AssetEntry* currentEntry);
		virtual void drawSideView(AssetEntry* currentEntry);
		virtual void drawMainView(AssetEntry* currentEntry);

		virtual void drawTree(const std::string& path);		
		virtual void openTree(const std::string& path);
		virtual void treeClicked(const std::string& path);
		virtual void contentClicked(const std::string& path, bool isFolder = false);
		virtual void addToBackQueue(const std::string& path);
		virtual void addToFrontQueue(const std::string& path);

	protected:

		std::string mTitle;
		float mIconSize{ 64.0f };
		float mIconPadding{ 16.0f };
		float mTextPadding{ 4.0f };
		Texture* mFolderIcon{ nullptr };
		Texture* mFileIcon{ nullptr };
		std::string mRootFolder;
		std::string mCurrentFolder;
		std::vector<std::unique_ptr<AssetEntry>> mEntries;
		std::unordered_map<std::string, uint32_t> mEntryMap;
		std::deque<std::string> mFrontQueue;
		std::deque<std::string> mBackQueue;
	};
}