#pragma once

#include "Core/Observer.h"
#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>

namespace Trinity
{
    enum class DisplayMode
    {
        Fullscreen,
        Borderless,
        Windowed
    };

    enum class Cursor : uint32_t
    {
        Arrow = 0,
        IBeam,
        VResize,
        HResize,
        Hand,
        ResizeAll,
        ResizeNESW,
        ResizeNWSE,
        NotAllowed,
        Max
    };

    struct WindowCallbacks
    {
        Observer<> onClose;
        Observer<int32_t, int32_t> onResize;
        Observer<int32_t, int32_t> onPos;
        Observer<bool> onFocus;
        Observer<int32_t, bool, int32_t> onKey;
        Observer<uint32_t> onChar;
        Observer<bool> onMouseEnter;
        Observer<float, float> onMousePos;
        Observer<int32_t, bool, int32_t> onMouseButton;
        Observer<float, float> onMouseScroll;
    };

    class Window
    {
    public:

        Window() = default;
        ~Window();

        Window(const Window&) = delete;
        Window& operator = (const Window&) = delete;

        Window(Window&&) = default;
        Window& operator = (Window&&) = default;

        GLFWwindow* getHandle() const
        {
            return mHandle;
        }

        uint32_t getWidth() const
        {
            return mWidth;
        }

        uint32_t getHeight() const
        {
            return mHeight;
        }

        DisplayMode getDisplayMode() const
        {
            return mDisplayMode;
        }

        WindowCallbacks& getCallbacks()
        {
            return mCallbacks;
        }

        virtual bool create(const std::string& title, uint32_t width, uint32_t height,
            DisplayMode displayMode);

        virtual void destroy();
        virtual bool isClosed() const;

        virtual void showMouse(bool visible, bool lock) const;
        virtual void show(bool visible) const;
        virtual void close();
        virtual void poll();

        virtual void setTitle(const std::string& title);
        virtual void setCursor(Cursor cursor) const;
        virtual void setSize(const glm::uvec2& size);

        virtual float getScaleFactor() const;
        virtual glm::uvec2 getSize() const;
        virtual glm::uvec2 getFramebufferSize() const;
        virtual std::unique_ptr<wgpu::ChainedStruct> getSurfaceDescriptor() const;

    protected:

        virtual void createCursors();

    public:

        static bool initialize();

    private:

        GLFWwindow* mHandle{ nullptr };
        uint32_t mWidth{ 0 };
        uint32_t mHeight{ 0 };
        DisplayMode mDisplayMode{ DisplayMode::Windowed };
        WindowCallbacks mCallbacks;
        std::vector<GLFWcursor*> mCursors;
    };
}