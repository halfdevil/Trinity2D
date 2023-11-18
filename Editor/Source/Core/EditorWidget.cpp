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
}