#pragma once

#include "Core/EditorWidget.h"
#include "Core/Observer.h"
#include "VFS/Storage.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <deque>
#include "glm/glm.hpp"

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
		static constexpr float kBaseIconSize = 32.0f;
		static constexpr float kBaseTextSize = 20.0f;
		static constexpr float kBaseIconPadding = 16.0f;
		static constexpr float kBaseTextPadding = 4.0f;

		struct AssetIcon
		{
			float size{ kBaseIconSize };
			float textSize{ kBaseTextSize };
			float iconPadding{ 16.0f };
			float textPadding{ 4.0f };
			uint32_t maxChars{ 12 };
		};

		AssetBrowser() = default;
		virtual ~AssetBrowser();

		AssetBrowser(const AssetBrowser&) = delete;
		AssetBrowser& operator = (const AssetBrowser&) = delete;

		AssetBrowser(AssetBrowser&&) = default;
		AssetBrowser& operator = (AssetBrowser&&) = default;

		const std::string& getRootFolder() const
		{
			return mRootFolder;
		}

		const std::string& getCurrentFolder() const
		{
			return mCurrentFolder;
		}

		bool isEmbedded() const
		{
			return mEmbedded;
		}

		const glm::vec2& getEmbeddedSize() const
		{
			return mEmbeddedSize;
		}

		virtual bool create(const std::string& rootFolder, EditorResources& resources);
		virtual void destroy();

		virtual void setEmbedded(bool embedded);
		virtual void setEmbeddedSize(const glm::vec2& embeddedSize);
		virtual void addFileExtension(const std::string& pattern);
		virtual void clearFileExtensions();

		virtual void refreshPath(const std::string& path);
		virtual void setTitle(const std::string& title) override;
		virtual void draw() override;

	protected:

		virtual AssetEntry* updateEntry(const std::string& folderPath);
		virtual AssetEntry* getEntry(const std::string& folderPath);

		virtual void drawTopView(AssetEntry* currentEntry);
		virtual void drawSideView(AssetEntry* currentEntry);
		virtual void drawMainView(AssetEntry* currentEntry);
		virtual void drawBreadCrumb(AssetEntry* currentEntry);

		virtual void drawTree(const std::string& path);		
		virtual void openTree(const std::string& path);
		virtual void treeClicked(const std::string& path);
		virtual void contentClicked(const std::string& path, bool isFolder = false);
		virtual void addToBackQueue(const std::string& path);
		virtual void addToFrontQueue(const std::string& path);

	public:

		Observer<const std::string&> onFileClicked;
		Observer<const std::string&> onFileDoubleClicked;

	protected:

		AssetIcon mAssetIcon;
		bool mEmbedded{ false };
		Texture* mFolderIcon{ nullptr };
		Texture* mFileIcon{ nullptr };
		std::string mRootFolder;
		std::string mCurrentFolder;
		std::vector<std::string> mFileExtensions;
		std::vector<std::unique_ptr<AssetEntry>> mEntries;
		std::unordered_map<std::string, uint32_t> mEntryMap;
		std::deque<std::string> mFrontQueue;
		std::deque<std::string> mBackQueue;
		std::string mDockspaceID;
		std::string mSideViewID;
		std::string mContentViewID;
		glm::vec2 mEmbeddedSize{ 0.0f };
	};
}