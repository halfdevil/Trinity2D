#include "Widgets/MessageBox.h"
#include "Graphics/Texture.h"
#include "Core/EditorResources.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/Logger.h"

namespace Trinity
{
	MessageBox::~MessageBox()
	{
		destroy();
	}

	bool MessageBox::create(float width, float height, EditorResources& resources)
	{
		auto& application = Application::get();
		auto* window = application.getWindow();
		auto scaleFactor = window->getScaleFactor();

		uint32_t iconSize = (uint32_t)std::floor(scaleFactor);

		mErrorIcon = resources.loadIcon(EditorIcon::Error, iconSize + 1);
		mWarningIcon = resources.loadIcon(EditorIcon::Warning, iconSize + 1);
		mInfoIcon = resources.loadIcon(EditorIcon::Info, iconSize + 1);

		if (!mErrorIcon || !mWarningIcon || !mInfoIcon)
		{
			LogError("Error loading icons for MessageBox");
			return false;
		}

		mEnabled = false;
		mButtonSize = { kBaseButtonSize.x * scaleFactor, kBaseButtonSize.y * scaleFactor };
		mBottomPanelPaddingY = kBaseBottomPanelPaddingY * scaleFactor;
		mIconPadding = kBaseIconPadding * scaleFactor;
		mTextPadding = kBaseTextPadding * scaleFactor;
		mDialogSize = { width * scaleFactor, height * scaleFactor };

		return true;
	}

	void MessageBox::destroy()
	{
		mErrorIcon = nullptr;
		mWarningIcon = nullptr;
		mInfoIcon = nullptr;
	}

	void MessageBox::draw()
	{
		if (!isEnabled())
		{
			return;
		}

		if (mShowing)
		{
			ImGui::OpenPopup(mTitle.c_str());
			mShowing = false;
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(mDialogSize);

		if (ImGui::BeginPopupModal(mTitle.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 availRegion = ImGui::GetContentRegionAvail();
			ImVec2 imageSize = { (float)mIcon->getWidth(), (float)mIcon->getHeight() };
			ImVec2 contentSize = { availRegion.x, availRegion.y - mBottomPanelSize.y - 2 * windowPadding.x };

			float cursorPosY = ImGui::GetCursorPosY();
			float cursorPosX = ImGui::GetCursorPosX();

			ImVec2 imagePosition = { 
				cursorPosX + mIconPadding, 
				cursorPosY + contentSize.y / 2 - imageSize.y / 2 
			};

			ImVec2 textPosition = {
				imagePosition.x + imageSize.x + mTextPadding,
				cursorPosY + contentSize.y / 2 - mMessageContentSize.y / 2
			};

			if (mTextSize.x < (cursorPosX + contentSize.x - textPosition.x))
			{
				textPosition.x = cursorPosX + contentSize.x / 2 - mTextSize.x / 2;
			}

			ImGui::Dummy(contentSize);
			ImGui::SetCursorPos(imagePosition);
			ImGui::Image(mIcon, imageSize);
			
			ImGui::SetCursorPos(textPosition);
			ImGui::BeginGroup();
			ImGui::PushTextWrapPos(cursorPosX + availRegion.x - 2 * windowPadding.x);
			ImGui::TextUnformatted(mMessage.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndGroup();
			mMessageContentSize = ImGui::GetItemRectSize();

			ImGui::SetCursorPosY(cursorPosY + contentSize.y);
			ImGui::BeginGroup();
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + mBottomPanelPaddingY);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availRegion.x / 2 - mButtonGroupWidth / 2);
			
			ImGui::BeginGroup();
			if (ImGui::Button(mButton1Text.c_str(), mButtonSize))
			{
				ImGui::CloseCurrentPopup();
				mEnabled = false;
				onClose(MessageBoxResult::Yes);
			}

			if (!mButton2Text.empty())
			{
				ImGui::SameLine();
				if (ImGui::Button(mButton2Text.c_str(), mButtonSize))
				{
					ImGui::CloseCurrentPopup();
					mEnabled = false;
					onClose(mButton2Text == "No" ? MessageBoxResult::No : MessageBoxResult::Cancel);
				}
			}

			if (!mButton3Text.empty())
			{
				ImGui::SameLine();
				if (ImGui::Button(mButton3Text.c_str(), mButtonSize))
				{
					ImGui::CloseCurrentPopup();
					mEnabled = false;
					onClose(MessageBoxResult::Cancel);
				}
			}

			ImGui::EndGroup();
			mButtonGroupWidth = ImGui::GetItemRectSize().x;

			ImGui::EndGroup();
			mBottomPanelSize = ImGui::GetItemRectSize();

			ImGui::EndPopup();
		}
	}

	void MessageBox::show(const std::string& message, const std::string& title, MessageBoxButtons buttons, MessageBoxIcon icon)
	{
		mMessage = message;
		mTextSize = ImGui::CalcTextSize(message.c_str());

		setTitle(title);

		switch (buttons)
		{
		case MessageBoxButtons::Ok:
			mButton1Text = "Ok";
			mButton2Text = "";
			mButton3Text = "";
			break;

		case MessageBoxButtons::OkCancel:
			mButton1Text = "Ok";
			mButton2Text = "Cancel";
			mButton3Text = "";
			break;

		case MessageBoxButtons::YesNo:
			mButton1Text = "Yes";
			mButton2Text = "No";
			mButton3Text = "";
			break;

		case MessageBoxButtons::YesNoCancel:
			mButton1Text = "Yes";
			mButton2Text = "No";
			mButton3Text = "Cancel";
			break;
		}

		switch (icon)
		{
		case MessageBoxIcon::Error:
			mIcon = mErrorIcon;
			break;

		case MessageBoxIcon::Warning:
			mIcon = mWarningIcon;
			break;

		case MessageBoxIcon::Info:
			mIcon = mInfoIcon;
			break;
		}

		mEnabled = true;
		mShowing = true;
	}

	void MessageBox::onClose(MessageBoxResult result)
	{
		onClosed.notify(result);
	}
}