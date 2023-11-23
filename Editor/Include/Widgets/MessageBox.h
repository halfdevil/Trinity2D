#pragma once

#include "Core/EditorWidget.h"
#include "Core/Observer.h"
#include <memory>

namespace Trinity
{
	class Texture;
	class EditorResources;

	enum class MessageBoxButtons
	{
		Ok,
		OkCancel,
		YesNo,
		YesNoCancel
	};

	enum class MessageBoxIcon
	{
		Error,
		Warning,
		Info
	};

	enum class MessageBoxResult
	{
		Yes,
		No,
		Cancel
	};

	class MessageBox : public EditorWidget
	{
	public:

		static constexpr ImVec2 kBaseButtonSize = ImVec2{ 100.0f, 36.0f };
		static constexpr float kBaseBottomPanelPaddingY = 10.0f;
		static constexpr float kBaseIconPadding = 16.0f;
		static constexpr float kBaseTextPadding = 16.0f;

		MessageBox() = default;
		virtual ~MessageBox();

		MessageBox(const MessageBox&) = delete;
		MessageBox& operator = (const MessageBox&) = delete;

		MessageBox(MessageBox&&) = default;
		MessageBox& operator = (MessageBox&&) = default;

		virtual bool create(float width, float height, EditorResources& resources);
		virtual void destroy();
		virtual void draw() override;

		virtual void show(
			const std::string& message, 
			const std::string& title, 
			MessageBoxButtons buttons = MessageBoxButtons::Ok,
			MessageBoxIcon icon = MessageBoxIcon::Info
		);

	protected:

		virtual void onClose(MessageBoxResult result);

	public:

		Observer<MessageBoxResult> onClosed;

	protected:

		bool mShowing{ false };
		std::string mMessage;
		Texture* mIcon{ nullptr };
		ImVec2 mDialogSize{ 0.0f, 0.0f };
		ImVec2 mBottomPanelSize{ 0.0f, 0.0f };
		ImVec2 mButtonSize{ 0.0f, 0.0f };
		ImVec2 mTextSize{ 0.0f, 0.0f };
		ImVec2 mMessageContentSize{ 0.0f, 0.0f };
		float mButtonGroupWidth{ 0.0f };
		float mBottomPanelPaddingY{ 0.0f };
		float mIconPadding{ 0.0f };
		float mTextPadding{ 0.0f };
		Texture* mErrorIcon{ nullptr };
		Texture* mWarningIcon{ nullptr };
		Texture* mInfoIcon{ nullptr };
		std::string mButton1Text;
		std::string mButton2Text;
		std::string mButton3Text;
	};
}