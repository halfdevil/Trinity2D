#include "Widgets/AssetFileDialog.h"
#include "Widgets/AssetBrowser.h"
#include "Core/EditorResources.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/Logger.h"
#include "VFS/FileSystem.h"
#include "misc/cpp/imgui_stdlib.h"
#include "IconsFontAwesome6.h"

namespace Trinity
{
	AssetFileDialog::~AssetFileDialog()
	{
		destroy();
	}

	bool AssetFileDialog::create(const std::string& rootFolder, EditorResources& resources)
	{
		mBrowser = std::make_unique<AssetBrowser>();
		if (!mBrowser->create(rootFolder, resources))
		{
			LogError("AssetBrowser::create() failed for: '%s'", rootFolder.c_str());
			return false;
		}

		mBrowser->setEmbedded(true);
		mBrowser->onFileClicked.subscribe([this](const std::string& path) {
			onFileClicked(path);
		});

		auto& application = Application::get();
		auto* window = application.getWindow();
		auto scaleFactor = window->getScaleFactor();

		mEnabled = false;
		mButtonSize = { kBaseButtonSize.x * scaleFactor, kBaseButtonSize.y * scaleFactor };
		mBottomPanelPaddingY = kBaseBottomPanelPaddingY * scaleFactor;

		setDialogType(AssetFileDialogType::Save);

		return true;
	}

	void AssetFileDialog::destroy()
	{
		mBrowser = nullptr;
	}

	void AssetFileDialog::show(AssetFileDialogType dialogType, const std::string& title)
	{
		setTitle(title);
		setDialogType(dialogType);

		mEnabled = mFileTypes.size() > 0;
		mOpened = mEnabled;
	}

	void AssetFileDialog::draw()
	{
		if (!isEnabled())
		{
			return;
		}

		if (mOpened)
		{
			ImGui::OpenPopup(mTitle.c_str());
			mOpened = false;
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal(mTitle.c_str(), nullptr))
		{
			ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 availRegion = ImGui::GetContentRegionAvail();

			if (mBrowser != nullptr)
			{
				mBrowser->setEmbeddedSize({ availRegion.x, availRegion.y - mBottomPanelSize.y - 2 * windowPadding.x });
				mBrowser->draw();
			}

			ImGui::BeginGroup();
			ImGui::Separator();
			ImGui::TextUnformatted("Filename:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::InputText("##fileName", &mFileName);
			ImGui::PopItemWidth();

			ImGui::TextUnformatted("File type:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

			if (ImGui::BeginCombo("##fileType", mFileTypes[mSelectedFileTypeIndex].name.c_str()))
			{
				for (uint32_t idx = 0; idx < (uint32_t)mFileTypes.size(); idx++)
				{
					bool isSelected = idx == mSelectedFileTypeIndex;
					if (ImGui::Selectable(mFileTypes[idx].name.c_str(), isSelected))
					{
						selectFileType(idx);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			ImGui::PopItemWidth();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + mBottomPanelPaddingY);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availRegion.x - mButtonGroupWidth);
			ImGui::BeginGroup();

			if (ImGui::Button(mButtonTitle.c_str(), mButtonSize))
			{
				ImGui::CloseCurrentPopup();
				mEnabled = false;
				onClose(AssetFileDialogResult::Success, mFileName);
			}

			ImGui::SameLine();

			if (ImGui::Button(ICON_FA_XMARK " Cancel", mButtonSize))
			{
				ImGui::CloseCurrentPopup();
				mEnabled = false;
				onClose(AssetFileDialogResult::Cancel, "");
			}

			ImGui::EndGroup();
			mButtonGroupWidth = ImGui::GetItemRectSize().x;

			ImGui::EndGroup();			
			mBottomPanelSize = ImGui::GetItemRectSize();

			ImGui::EndPopup();
		}
	}

	void AssetFileDialog::setTitle(const std::string& title)
	{
		EditorWidget::setTitle(title);

		if (mBrowser != nullptr)
		{
			mBrowser->setTitle("##assetBrowser-" + title);
		}
	}

	void AssetFileDialog::setDialogType(AssetFileDialogType dialogType)
	{
		mDialogType = dialogType;

		switch (dialogType)
		{
		case AssetFileDialogType::Open:
			mButtonTitle = ICON_FA_RIGHT_TO_BRACKET " Open";
			break;

		case AssetFileDialogType::Save:
			mButtonTitle = ICON_FA_FLOPPY_DISK " Save";
			break;

		case AssetFileDialogType::SaveAs:
			mButtonTitle = ICON_FA_FLOPPY_DISK " Save";
			break;

		default:
			break;
		}
	}

	void AssetFileDialog::addFileType(const FileType& fileType)
	{
		mFileTypes.push_back(fileType);

		if (mFileTypes.size() == 1)
		{
			selectFileType(0);
		}
	}

	void AssetFileDialog::addFileType(const std::string& name, std::vector<std::string>&& extensions)
	{
		FileType fileType = {
			name,
			std::move(extensions)
		};

		addFileType(fileType);
	}

	void AssetFileDialog::selectFileType(uint32_t fileTypeIndex)
	{
		mSelectedFileTypeIndex = fileTypeIndex;
		if (mBrowser != nullptr)
		{
			mBrowser->clearFileExtensions();
			for (auto& extension : mFileTypes[mSelectedFileTypeIndex].extensions)
			{
				mBrowser->addFileExtension(extension);
			}
		}
	}

	void AssetFileDialog::onFileClicked(const std::string& path)
	{
		auto& fileSystem = FileSystem::get();
		mFileName = fileSystem.getFileName(path);
	}

	void AssetFileDialog::onClose(AssetFileDialogResult result, const std::string& fileName)
	{
		if (!fileName.empty())
		{
			auto& fileSystem = FileSystem::get();
			auto path = fileSystem.combinePath(mBrowser->getCurrentFolder(), fileName);

			onClosed.notify(mDialogType, result, path);
		}
		else
		{
			onClosed.notify(mDialogType, result, fileName);
		}
	}
}