#include "Widgets/AssetBrowser.h"
#include "EditorResources.h"
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
		mIconsFont = resources.getFont("font-64");
		mRootFolder = rootFolder;
		mCurrentFolder = rootFolder;

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
		auto* currentEntry = getEntry(mCurrentFolder);
		if (!currentEntry)
		{
			currentEntry = updateEntry(mCurrentFolder);
		}

		ImGui::Begin(mTitle.c_str());
		{
			ImGuiID dockspaceID = ImGui::GetID("##assetBrowser-dockspace");			
			if (!ImGui::DockBuilderGetNode(dockspaceID))
			{
				ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
				ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetWindowSize());

				ImGuiID rightID;
				ImGuiID leftID = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left,
					0.25f, &leftID, &rightID);

				ImGuiID bottomID;
				ImGuiID topID = ImGui::DockBuilderSplitNode(rightID, ImGuiDir_Up,
					0.15f, &topID, &bottomID);

				ImGui::DockBuilderDockWindow("##assetBrowser-tree", leftID);
				ImGui::DockBuilderDockWindow("##assetBrowser-topBar", topID);
				ImGui::DockBuilderDockWindow("##assetBrowser-content", bottomID);
				ImGui::DockBuilderFinish(dockspaceID);
			}

			ImGui::DockSpace(dockspaceID, { 0, 0 }, ImGuiDockNodeFlags_NoTabBar |
				ImGuiDockNodeFlags_NoDocking);

			ImGui::Begin("##assetBrowser-topBar", nullptr, ImGuiWindowFlags_HorizontalScrollbar |
				ImGuiWindowFlags_NoMove);
			{
				ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});

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

				ImGui::End();
			}

			ImGui::Begin("##assetBrowser-content", nullptr, ImGuiWindowFlags_HorizontalScrollbar |
				ImGuiWindowFlags_NoMove);
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
						mIconsFont->activate();
						ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });

						ImGui::ButtonEx(fileEntry.directory ? ICON_FA_FOLDER : ICON_FA_FILE_LINES, 
							{ mIconSize + mIconPadding / 2.0f, mIconSize + mIconPadding / 2.0f });

						ImGui::PopStyleColor();
						mIconsFont->deactivate();

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
				ImGui::End();
			}

			ImGui::Begin("##assetBrowser-tree", nullptr, ImGuiWindowFlags_HorizontalScrollbar |
				ImGuiWindowFlags_NoMove);
			{
				drawTree(mRootFolder);
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

		std::string label = isOpen ? ICON_FA_FOLDER_OPEN " " : ICON_FA_FOLDER_CLOSED "  ";
		label.append(entry->name);

		bool hasOpen = ImGui::TreeNodeEx(&entry, flags, "%s", label.c_str());
		if (ImGui::IsItemClicked())
		{
			if (mCurrentFolder != entry->path)
			{
				std::string entryPath{ entry->path };
				treeClicked(entryPath);

				entry = getEntry(entryPath);
				entry->isOpen = true;
			}
		}

		if (hasOpen && !isLeaf)
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
		else
		{
			if (!isLeaf)
			{
				entry->isOpen = false;
			}
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