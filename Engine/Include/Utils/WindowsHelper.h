#pragma once

#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>
#include <memory>

namespace Trinity
{
	class WindowsHelper
	{
	public:

		static std::unique_ptr<wgpu::ChainedStruct> getWin32Surface(GLFWwindow* handle);
	};
}