#ifdef _WIN32
#include "Utils/WindowsHelper.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Trinity
{
	std::unique_ptr<wgpu::ChainedStruct> WindowsHelper::getWin32Surface(GLFWwindow* handle)
	{
		auto surfaceDesc = std::make_unique<wgpu::SurfaceDescriptorFromWindowsHWND>();
		surfaceDesc->hwnd = glfwGetWin32Window(handle);
		surfaceDesc->hinstance = GetModuleHandle(nullptr);
		return surfaceDesc;
	}
}
#endif