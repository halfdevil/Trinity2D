#include "Gui/WidgetEvent.h"

namespace Trinity
{
	void WidgetEvent::setType(WidgetEventType type)
	{
		mType = type;
	}

	void WidgetEvent::setData(const WidgetEventData& data)
	{
		mData = data;
	}
}