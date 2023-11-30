#pragma once

#include "Input/Types.h"
#include "Core/Observer.h"
#include <cstdint>

namespace Trinity
{
    class Keyboard
    {
    public:

        Keyboard();

        Keyboard(const Keyboard&) = delete;
        Keyboard& operator = (const Keyboard&) = delete;

        Keyboard(Keyboard&&) = delete;
        Keyboard& operator = (Keyboard&&) = delete;

        virtual bool isKeyDown(int32_t key) const;
        virtual bool isKeyUp(int32_t key) const;
        virtual bool isKeyHeld(int32_t key) const;
        virtual bool isKeyiggered(int32_t key) const;

        virtual void update(int32_t key, bool pressed);
        virtual void postUpdate();

    public:

        Observer<int32_t, bool> onKeyStateUpdated;

    private:

        bool mCurrentState[KEY_LAST];
        bool mOldState[KEY_LAST];
    };
}