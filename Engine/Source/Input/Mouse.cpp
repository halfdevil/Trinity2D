#include "Input/Mouse.h"

namespace Trinity
{
    Mouse::Mouse()
    {
        std::memset(mCurrentState, 0, sizeof(mCurrentState));
        std::memset(mOldState, 0, sizeof(mOldState));
    }

    bool Mouse::isButtonDown(int32_t button) const
    {
        if (button >= 0 && button < MOUSE_BUTTON_LAST)
        {
            return mCurrentState[button];
        }

        return false;
    }

    bool Mouse::isButtonUp(int32_t button) const
    {
        if (button >= 0 && button < MOUSE_BUTTON_LAST)
        {
            return !mCurrentState[button];
        }

        return false;
    }

    bool Mouse::isButtonHeld(int32_t button) const
    {
        if (button >= 0 && button < MOUSE_BUTTON_LAST)
        {
            return mCurrentState[button] && mOldState[button];
        }

        return false;
    }

    bool Mouse::isButtoniggered(int32_t button) const
    {
        if (button >= 0 && button < MOUSE_BUTTON_LAST)
        {
            return mCurrentState[button] && !mOldState[button];
        }

        return false;
    }

    void Mouse::update(int32_t button, bool pressed)
    {
        mCurrentState[button] = pressed;
        onButtonStateUpdated.notify(button, pressed);
    }

    void Mouse::updatePosition(float x, float y)
    {
        mCurrentPosition = { x , y };
        onPositionUpdated.notify(x, y);
    }

    void Mouse::updateScroll(float x, float y)
    {
        mScroll = { x, y };
        onScrollUpdated.notify(x, y);
    }

    void Mouse::postUpdate()
    {
        memcpy(mOldState, mCurrentState, sizeof(bool) * MOUSE_BUTTON_LAST);
        mOldPosition = mCurrentPosition;
    }
}