#pragma once

#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>
#include <memory>

namespace Trinity
{
    class MacOSHelper
    {
    public:

        static std::unique_ptr<wgpu::ChainedStruct> getMetalSurface(GLFWwindow* handle);
    };
}