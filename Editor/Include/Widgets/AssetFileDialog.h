#pragma once

#include "Core/EditorWidget.h"
#include "Core/Observer.h"
#include <memory>

namespace Trinity
{
	class AssetBrowser;
	class EditorResources;

	enum class AssetFileDialogType
	{
		Open,
		Save,
		SaveAs
	};

	enum class AssetFileDialogResult
	{
		Success,
		Cancel
	};

	class AssetFileDialog : public EditorWidget
	{
	public:

		static constexpr ImVec2 kBaseButtonSize = ImVec2{ 100.0f, 36.0f };
		static constexpr float kBaseBottomPanelPaddingY = 10.0f;

		struct FileType
		{
			std::string name;
			std::vector<std::string> extensions;
		};

		AssetFileDialog() = default;
		virtual ~AssetFileDialog();

		AssetFileDialog(const AssetFileDialog&) = delete;
		AssetFileDialog& operator = (const AssetFileDialog&) = delete;

		AssetFileDialog(AssetFileDialog&&) = default;
		AssetFileDialog& operator = (AssetFileDialog&&) = default;

		const std::string& getFileName() const
		{
			return mFileName;
		}

		virtual bool create(const std::string& rootFolder, EditorResources& resources);
		virtual void destroy();

		virtual void show(AssetFileDialogType dialogType, const std::string& title);
		virtual void draw() override;

		virtual void setTitle(const std::string& title);
		virtual void addFileType(const FileType& fileType);
		virtual void addFileType(const std::string& name, std::vector<std::string>&& extensions);

	protected:

		virtual void setDialogType(AssetFileDialogType dialogType);
		virtual void selectFileType(uint32_t fileTypeIndex);
		virtual void onFileClicked(const std::string& path);
		virtual void onClose(AssetFileDialogResult result, const std::string& fileName);

	public:

		Observer<AssetFileDialogType, AssetFileDialogResult, const std::string&> onClosed;

	protected:

		AssetFileDialogType mDialogType{ AssetFileDialogType::Save };
		std::string mFileName;
		bool mOpened{ false };
		std::unique_ptr<AssetBrowser> mBrowser{ nullptr };
		std::vector<FileType> mFileTypes;
		uint32_t mSelectedFileTypeIndex{ 0 };
		ImVec2 mBottomPanelSize{ 0.0f, 0.0f };
		ImVec2 mButtonSize{ 0.0f, 0.0f };
		float mButtonGroupWidth{ 0.0f };
		float mBottomPanelPaddingY{ 0.0f };
		std::string mButtonTitle;
	};
}