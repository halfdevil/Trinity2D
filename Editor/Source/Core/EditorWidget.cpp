#include "Core/EditorWidget.h"

namespace Trinity
{
	bool EditorWidget::isEnabled() const
	{
		return mEnabled;
	}

	bool EditorWidget::isActive() const
	{
		return mActive;
	}

	void EditorWidget::setEnabled(bool enabled)
	{
		mEnabled = enabled;
	}

	void EditorWidget::setActive(bool active)
	{
		mActive = active;
	}

	void EditorWidget::setTitle(const std::string& title)
	{
		mTitle = title;
	}
}