#pragma once

#include "EditorWidget.h"
#include "VFS/Storage.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace Trinity
{
	class EditorCache;
	class Texture;

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

		static constexpr const char* kFolderIcon = "/Assets/Textures/Icons/Folder.png";
		static constexpr const char* kFolderOpenIcon = "/Assets/Textures/Icons/FolderOpen.png";
		static constexpr const char* kFileIcon = "/Assets/Textures/Icons/File.png";

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

		virtual bool create(const std::string& rootFolder, EditorCache& cache);
		virtual void destroy();

		virtual void setTitle(const std::string& title);
		virtual void draw() override;

	protected:

		virtual bool updateEntry(const std::string& folderPath);
		virtual void drawEntry(const std::string& path);
		virtual void assetClicked(const std::string& path, bool isFolder = false);
		virtual void openTree(const std::string& path);

	protected:

		std::string mTitle;
		std::string mRootFolder;
		std::string mCurrentFolder;
		Texture* mFolderIcon{ nullptr };
		Texture* mFolderOpenIcon{ nullptr };
		Texture* mFileIcon{ nullptr };
		float mIconSize{ 64.0f };
		float mIconPadding{ 16.0f };
		float mTextPadding{ 4.0f };
		AssetEntry* mRootEntry{ nullptr };
		AssetEntry* mCurrentEntry{ nullptr };
		std::vector<std::unique_ptr<AssetEntry>> mEntries;
		std::unordered_map<std::string, uint32_t> mEntryMap;
	};
}