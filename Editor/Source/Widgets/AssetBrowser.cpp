#include "Widgets/AssetBrowser.h"
#include "EditorCache.h"
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

	bool AssetBrowser::create(const std::string& rootFolder, EditorCache& cache)
	{
		if (!updateEntry(rootFolder))
		{
			LogError("AssetBrowser::updateEntry() failed for: '%s'", rootFolder.c_str());
			return false;
		}

		mCurrentEntry = mEntries.back().get();
		mRootEntry = mCurrentEntry;
		mFolderIcon = cache.loadTexture(kFolderIcon);
		mFolderOpenIcon = cache.loadTexture(kFolderOpenIcon);
		mFileIcon = cache.loadTexture(kFileIcon);

		mRootFolder = rootFolder;
		mCurrentFolder = rootFolder;
		mRootEntry->isOpen = true;

		return true;
	}

	void AssetBrowser::destroy()
	{
		mEntries.clear();
		mEntryMap.clear();
	}

	void AssetBrowser::setTitle(const std::string& title)
	{
		mTitle = title;
	}

	void AssetBrowser::draw()
	{
		if (ImGui::Begin(mTitle.c_str()))
		{
			ImGuiID dockspaceID = ImGui::GetID("##assetBrowser-dockspace");
			
			if (!ImGui::DockBuilderGetNode(dockspaceID))
			{
				ImGui::DockBuilderRemoveNode(dockspaceID);
				ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
				ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetWindowSize());

				ImGuiID rightID;
				ImGuiID leftID = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left,
					0.25f, &leftID, &rightID);

				ImGuiID bottomID;
				ImGuiID topID = ImGui::DockBuilderSplitNode(rightID, ImGuiDir_Up,
					0.10f, &topID, &bottomID);

				ImGui::DockBuilderDockWindow("##assetBrowser-tree", leftID);
				ImGui::DockBuilderDockWindow("##assetBrowser-topBar", topID);
				ImGui::DockBuilderDockWindow("##assetBrowser-content", bottomID);
				ImGui::DockBuilderFinish(dockspaceID);
			}

			ImGui::DockSpace(dockspaceID, { 0, 0 }, ImGuiDockNodeFlags_NoTabBar |
				ImGuiDockNodeFlags_NoDocking);

			if (ImGui::Begin("##assetBrowser-topBar", nullptr, ImGuiWindowFlags_HorizontalScrollbar |
				ImGuiWindowFlags_NoMove))
			{
				if (ImGui::ArrowButton("##assetBrowser-previous", ImGuiDir_Left))
				{
				}

				ImGui::SameLine();

				if (ImGui::ArrowButton("##assetBrowser-next", ImGuiDir_Right))
				{
				}

				ImGui::SameLine();
				ImGui::TextUnformatted(mCurrentEntry->name.c_str());
				ImGui::End();
			}

			if (ImGui::Begin("##assetBrowser-content", nullptr, ImGuiWindowFlags_HorizontalScrollbar |
				ImGuiWindowFlags_NoMove))
			{
				const float assetSize = mIconSize + mIconPadding;
				float contentSize = ImGui::GetContentRegionAvail().x;

				int32_t contentId = 0;
				int32_t numColumns = std::max((int32_t)(contentSize / assetSize), 1);
				ImGui::Columns(numColumns, nullptr, false);

				if (mCurrentEntry != nullptr)
				{
					for (auto& fileEntry : mCurrentEntry->fileEntries)
					{
						Texture* texture = fileEntry.directory ? mFolderIcon : mFileIcon;

						ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
						ImGui::ImageButtonEx(ImGui::GetID(&fileEntry), texture, { mIconSize, mIconSize }, { 0, 0 },
							{ 1, 1 }, { 0, 0, 0, 0 }, { 1, 1, 1, 1 });
						ImGui::PopStyleColor();

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							assetClicked(fileEntry.path, fileEntry.directory);
						}

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + mTextPadding);
						ImGui::TextWrapped(fileEntry.name.c_str());
						ImGui::NextColumn();
					}
				}

				ImGui::Columns(1);
				ImGui::End();
			}

			if (mRootEntry != nullptr)
			{
				if (ImGui::Begin("##assetBrowser-tree", nullptr, ImGuiWindowFlags_HorizontalScrollbar |
					ImGuiWindowFlags_NoMove))
				{
					drawEntry(mRootEntry->path);
					ImGui::End();
				}
			}

			ImGui::End();
		}
	}

	bool AssetBrowser::updateEntry(const std::string& folderPath)
	{
		auto& fileSystem = FileSystem::get();

		std::vector<FileEntry> entries;
		if (!fileSystem.getFiles(folderPath, false, entries))
		{
			LogError("FileSystem::getFiles() failed for: '%s'", folderPath.c_str());
			return false;
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

		if (auto it = mEntryMap.find(folderPath); it != mEntryMap.end())
		{
			if (mCurrentEntry->path == folderPath)
			{
				mCurrentEntry = entry.get();
			}

			mEntries[it->second] = std::move(entry);
		}
		else
		{
			mEntries.push_back(std::move(entry));
			mEntryMap.insert(std::make_pair(folderPath, (uint32_t)mEntries.size() - 1));
		}

		return true;
	}

	void AssetBrowser::drawEntry(const std::string& path)
	{
		if (auto it = mEntryMap.find(path); it == mEntryMap.end())
		{
			updateEntry(path);
		}

		if (auto it = mEntryMap.find(path); it != mEntryMap.end())
		{
			auto flags = ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_SpanFullWidth;

			auto* entry = mEntries[it->second].get();
			if (entry->isOpen)
			{
				flags |= ImGuiTreeNodeFlags_DefaultOpen;
			}

			if (!entry->hasFolders)
			{
				flags |= ImGuiTreeNodeFlags_Leaf;

				std::string label = entry->isOpen ? ICON_FA_FOLDER_OPEN " " : ICON_FA_FOLDER_CLOSED "  ";
				label.append(entry->name);

				ImGui::TreeNodeEx(&entry, flags, label.c_str());

				if (ImGui::IsItemClicked())
				{
					entry->isOpen = true;
					auto path(entry->path);

					assetClicked(path, true);
				}

				ImGui::TreePop();
			}
			else
			{
				std::string label = entry->isOpen ? ICON_FA_FOLDER_OPEN " " : ICON_FA_FOLDER_CLOSED "  ";
				label.append(entry->name);

				if (ImGui::TreeNodeEx(&entry, flags, label.c_str()))
				{
					entry->isOpen = true;

					for (auto& fileEntry : entry->fileEntries)
					{
						if (!fileEntry.directory)
						{
							continue;
						}

						drawEntry(fileEntry.path);
					}

					ImGui::TreePop();
				}
				else
				{
					entry->isOpen = false;
				}
			}
		}
	}

	void AssetBrowser::assetClicked(const std::string& path, bool isFolder)
	{
		if (isFolder && updateEntry(path))
		{
			if (auto it = mEntryMap.find(path); it != mEntryMap.end())
			{
				mCurrentEntry = mEntries[it->second].get();
				openTree(path);
			}
		}
	}

	void AssetBrowser::openTree(const std::string& path)
	{
		auto& fileSystem = FileSystem::get();
		auto diff = fileSystem.subtractPath(mCurrentEntry->path, mRootEntry->path);

		auto parts = StringHelper::split(diff, '/');
		auto newPath(mRootEntry->path);

		for (auto& part : parts)
		{
			newPath = fileSystem.combinePath(newPath, part);
			newPath = fileSystem.sanitizePath(newPath);

			updateEntry(newPath);

			if (auto it = mEntryMap.find(newPath); it != mEntryMap.end())
			{
				auto& entry = *mEntries[it->second];
				entry.isOpen = true;
			}
		}
	}
}