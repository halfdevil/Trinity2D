#pragma once

#include "Input/Types.h"
#include "Core/Observer.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace Trinity
{
    class Mouse
    {
    public:

        Mouse();
        ~Mouse() = default;

        Mouse(const Mouse&) = delete;
        Mouse& operator = (const Mouse&) = delete;

        Mouse(Mouse&&) = delete;
        Mouse& operator = (Mouse&&) = delete;

        const glm::vec2& getPosition() const
        {
            return mCurrentPosition;
        }

        const glm::vec2& getOldPosition() const
        {
            return mOldPosition;
        }

        glm::ivec2 getRelativePosition() const
        {
            return mCurrentPosition - mOldPosition;
        }

        const glm::vec2& getScroll() const
        {
            return mScroll;
        }

        virtual bool isButtonDown(int32_t button) const;
        virtual bool isButtonUp(int32_t button) const;
        virtual bool isButtonHeld(int32_t button) const;
        virtual bool isButtonTriggered(int32_t button) const;

        virtual void update(int32_t button, bool pressed);
        virtual void updatePosition(float x, float y);
        virtual void updateScroll(float x, float y);
        virtual void postUpdate();

    public:

        Observer<int32_t, bool> onButtonStateUpdated;
        Observer<float, float> onPositionUpdated;
        Observer<float, float> onScrollUpdated;

    private:

        bool mCurrentState[MOUSE_BUTTON_LAST];
        bool mOldState[MOUSE_BUTTON_LAST];
        glm::vec2 mCurrentPosition;
        glm::vec2 mOldPosition;
        glm::vec2 mScroll;
    };
}