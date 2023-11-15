#include "Utils/MacOSHelper.h"

#ifdef __APPLE__
#include <Foundation/Foundation.h>
#include <QuartzCore/CAMetalLayer.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#endif

namespace Trinity
{
    std::unique_ptr<wgpu::ChainedStruct> MacOSHelper::getMetalSurface(GLFWwindow* handle)
    {
        id metalLayer = NULL;
        NSWindow* nsWindow = glfwGetCocoaWindow(handle);
        [nsWindow.contentView setWantsLayer: YES];
        metalLayer = [CAMetalLayer layer];
        [nsWindow.contentView setLayer: metalLayer];

        auto surfaceDesc = std::make_unique<wgpu::SurfaceDescriptorFromMetalLayer>();  
        surfaceDesc->layer = metalLayer;
        return surfaceDesc;
    }
}