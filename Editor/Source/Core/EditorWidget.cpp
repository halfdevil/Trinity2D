#include "Core/EditorWidget.h"

namespace Trinity
{
	bool EditorWidget::isEnabled() const
	{
		return mEnabled;
	}

	void EditorWidget::setEnabled(bool enabled)
	{
		mEnabled = enabled;
	}

	void EditorWidget::setTitle(const std::string& title)
	{
		mTitle = title;
	}
}