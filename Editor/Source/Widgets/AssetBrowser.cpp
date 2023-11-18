#include "Widgets/AssetBrowser.h"
#include "Core/EditorResources.h"
#include "ImGui/ImGuiFont.h"
#include "ImGui/ImGuiRenderer.h"
#include "Graphics/Texture.h"
#include "VFS/FileSystem.h"
#include "Core/Logger.h"
#include "Utils/StringHelper.h"
#include "imgui_internal.h"
#include "IconsFontAwesome6.h"

namespace Trinity
{
	AssetBrowser::~AssetBrowser()
	{
		destroy();
	}

	bool AssetBrowser::create(const std::string& rootFolder, EditorResources& resources)
	{
		auto* entry = updateEntry(rootFolder);
		if (!entry)
		{
			LogError("AssetBrowser::updateEntry() failed for: '%s'", rootFolder.c_str());
			return false;
		}

		entry->isOpen = true;
		mRootFolder = rootFolder;
		mCurrentFolder = rootFolder;

		mFolderIcon = resources.loadIcon(EditorIcon::Folder, 64);
		mFileIcon = resources.loadIcon(EditorIcon::File, 64);

		return true;
	}

	void AssetBrowser::destroy()
	{
		mEntries.clear();
		mEntryMap.clear();
	}

	void AssetBrowser::draw()
	{
		auto* currentEntry = getEntry(mCurrentFolder);
		if (!currentEntry)
		{
			currentEntry = updateEntry(mCurrentFolder);
		}

		if (ImGui::Begin(mTitle.c_str()))
		{
			ImGuiID dockspaceID = ImGui::GetID("##assetBrowser-dockspace");
			if (!ImGui::DockBuilderGetNode(dockspaceID))
			{
				ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
				ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetWindowSize());

				ImGuiID mainID;
				ImGuiID sideID = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left,
					0.25f, &sideID, &mainID);

				ImGui::DockBuilderDockWindow("##assetBrowser-side", sideID);
				ImGui::DockBuilderDockWindow("##assetBrowser-main", mainID);

				ImGui::DockBuilderFinish(dockspaceID);
			}

			ImGui::DockSpace(dockspaceID, { 0, 0 }, ImGuiDockNodeFlags_NoTabBar |
				ImGuiDockNodeFlags_NoDocking);

			ImGui::Begin("##assetBrowser-main", nullptr, ImGuiWindowFlags_HorizontalScrollbar |
				ImGuiWindowFlags_NoMove);
			{
				drawTopView(currentEntry);
				ImGui::Separator();
				drawMainView(currentEntry);

				ImGui::End();
			}

			ImGui::Begin("##assetBrowser-side", nullptr, ImGuiWindowFlags_HorizontalScrollbar |
				ImGuiWindowFlags_NoMove);
			{
				drawSideView(currentEntry);
				ImGui::End();
			}

			ImGui::End();
		}
	}

	AssetEntry* AssetBrowser::updateEntry(const std::string& folderPath)
	{
		auto& fileSystem = FileSystem::get();

		std::vector<FileEntry> entries;
		if (!fileSystem.getFiles(folderPath, false, entries))
		{
			LogError("FileSystem::getFiles() failed for: '%s'", folderPath.c_str());
			return nullptr;
		}

		bool hasFolders{ false };
		for (auto& fileEntry : entries)
		{
			hasFolders |= fileEntry.directory;
		}

		auto entry = std::make_unique<AssetEntry>(
			fileSystem.getFileName(folderPath),
			folderPath, 
			true, 
			hasFolders, 
			std::move(entries)
		);

		auto* entryPtr = entry.get();
		if (auto it = mEntryMap.find(folderPath); it != mEntryMap.end())
		{
			mEntries[it->second] = std::move(entry);
		}
		else
		{
			mEntries.push_back(std::move(entry));
			mEntryMap.insert(std::make_pair(folderPath, (uint32_t)mEntries.size() - 1));
		}

		return entryPtr;
	}

	AssetEntry* AssetBrowser::getEntry(const std::string& folderPath)
	{
		if (auto it = mEntryMap.find(folderPath); it != mEntryMap.end())
		{
			return mEntries[it->second].get();
		}

		return nullptr;
	}

	void AssetBrowser::drawTopView(AssetEntry* currentEntry)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });

		bool disabled = mBackQueue.empty();
		if (disabled)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button(ICON_FA_ARROW_LEFT))
		{
			addToFrontQueue(mCurrentFolder);

			mCurrentFolder = mBackQueue.front();
			mBackQueue.pop_front();

			currentEntry = updateEntry(mCurrentFolder);
			currentEntry->isOpen = true;
		}

		if (disabled)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		ImGui::SameLine();

		disabled = mFrontQueue.empty();
		if (disabled)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button(ICON_FA_ARROW_RIGHT))
		{
			addToBackQueue(mCurrentFolder);

			mCurrentFolder = mFrontQueue.front();
			mFrontQueue.pop_front();

			currentEntry = updateEntry(mCurrentFolder);
			currentEntry->isOpen = true;
		}

		if (disabled)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		ImGui::PopStyleColor();

		if (currentEntry != nullptr)
		{
			ImGui::SameLine();
			ImGui::TextUnformatted(currentEntry->name.c_str());
		}
	}

	void AssetBrowser::drawSideView(AssetEntry* currentEntry)
	{
		drawTree(mRootFolder);
	}

	void AssetBrowser::drawMainView(AssetEntry* currentEntry)
	{
		const float assetSize = mIconSize + mIconPadding;
		float contentSize = ImGui::GetContentRegionAvail().x;

		int32_t contentId = 0;
		int32_t numColumns = std::max((int32_t)(contentSize / assetSize), 1);
		ImGui::Columns(numColumns, nullptr, false);

		if (currentEntry != nullptr)
		{
			for (auto& fileEntry : currentEntry->fileEntries)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
				ImGui::ImageButtonEx(
					ImGui::GetID(&fileEntry),
					fileEntry.directory ? mFolderIcon : mFileIcon,
					{ mIconSize, mIconSize },
					{ 0, 0 },
					{ 1, 1 },
					{ 0, 0, 0, 0 },
					{ 1, 1, 1, 1 }
				);

				ImGui::PopStyleColor();

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					contentClicked(fileEntry.path, fileEntry.directory);
				}

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + mTextPadding);
				ImGui::TextWrapped("%s", fileEntry.name.c_str());
				ImGui::NextColumn();
			}
		}

		ImGui::Columns(1);
	}

	void AssetBrowser::drawTree(const std::string& path)
	{
		AssetEntry* entry = getEntry(path);
		if (!entry)
		{
			entry = updateEntry(path);
		}

		auto flags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_SpanFullWidth;

		bool isOpen = entry->isOpen;
		bool isLeaf = !entry->hasFolders;
		bool isClicked = false;

		if (isOpen)
		{
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		if (isLeaf)
		{
			flags |= (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
		}

		if (mCurrentFolder == entry->path)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		std::string label = isOpen ? ICON_FA_FOLDER_OPEN " " : ICON_FA_FOLDER_CLOSED " ";
		label.append(entry->name);

		bool hasOpen = ImGui::TreeNodeEx(entry, flags, "%s", label.c_str());
		if (ImGui::IsItemClicked() && (!ImGui::IsItemToggledOpen() || isLeaf))
		{
			if (mCurrentFolder != entry->path)
			{
				std::string entryPath{ entry->path };
				treeClicked(entryPath);

				entry = getEntry(entryPath);
				entry->isOpen = true;
				isClicked = true;
			}
		}

		if (hasOpen)
		{
			if (!isLeaf)
			{
				entry->isOpen = true;
				for (auto& fileEntry : entry->fileEntries)
				{
					if (!fileEntry.directory)
					{
						continue;
					}

					drawTree(fileEntry.path);
				}

				ImGui::TreePop();
			}
		}
		else
		{
			entry->isOpen = isClicked;
		}
	}

	void AssetBrowser::openTree(const std::string& path)
	{
		auto& fileSystem = FileSystem::get();
		auto diff = fileSystem.subtractPath(mCurrentFolder, mRootFolder);

		auto parts = StringHelper::split(diff, '/');
		auto newPath(mRootFolder);

		for (auto& part : parts)
		{
			newPath = fileSystem.combinePath(newPath, part);
			newPath = fileSystem.sanitizePath(newPath);

			auto* entry = updateEntry(newPath);
			if (entry != nullptr)
			{
				entry->isOpen = true;
			}
		}
	}

	void AssetBrowser::treeClicked(const std::string& path)
	{
		auto* entry = updateEntry(path);
		if (entry != nullptr)
		{
			addToBackQueue(mCurrentFolder);
			mCurrentFolder = path;
		}
	}

	void AssetBrowser::contentClicked(const std::string& path, bool isFolder)
	{
		if (isFolder)
		{
			auto* entry = updateEntry(path);
			if (entry != nullptr)
			{
				addToBackQueue(mCurrentFolder);
				mCurrentFolder = path;

				openTree(path);
			}
		}
	}

	void AssetBrowser::addToBackQueue(const std::string& path)
	{
		if ((uint32_t)mBackQueue.size() >= kMaxQueueSize)
		{
			mBackQueue.pop_back();
		}

		mBackQueue.push_front(path);
	}

	void AssetBrowser::addToFrontQueue(const std::string& path)
	{
		if ((uint32_t)mFrontQueue.size() >= kMaxQueueSize)
		{
			mFrontQueue.pop_back();
		}

		mFrontQueue.push_front(path);
	}
}