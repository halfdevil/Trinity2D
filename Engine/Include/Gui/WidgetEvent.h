#pragma once

#include "glm/glm.hpp"

namespace Trinity
{
	enum class WidgetEventType
	{
		Undefined,
		GotFocus,
		LostFocus,
		KeyDown,
		KeyUp,
		MouseDown,
		MouseUp,
		MouseEnter,
		MouseLeave,
		MouseMove
	};

	struct WidgetEventData
	{
		glm::vec2 mousePosition{ 0.0f };
		int32_t mouseButton{ -1 };
		int32_t key{ -1 };
	};

	class WidgetEvent
	{
	public:

		WidgetEvent() = default;
		virtual ~WidgetEvent() = default;

		WidgetEvent(const WidgetEvent&) = delete;
		WidgetEvent& operator = (const WidgetEvent&) = delete;

		WidgetEvent(WidgetEvent&&) = default;
		WidgetEvent& operator = (WidgetEvent&&) = default;

		WidgetEventType getType() const
		{
			return mType;
		}

		WidgetEventData& getData()
		{
			return mData;
		}

		virtual void setType(WidgetEventType type);
		virtual void setData(const WidgetEventData& data);

	protected:

		WidgetEventType mType{ WidgetEventType::Undefined };
		WidgetEventData mData;
	};
}