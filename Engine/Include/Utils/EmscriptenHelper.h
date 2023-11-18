#pragma once

#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>
#include <memory>
#include <string>

namespace Trinity
{
	class EmscriptenHelper
	{
	public:

		static std::unique_ptr<wgpu::ChainedStruct> getCanvasSurface(const std::string& selector, GLFWwindow* handle);
	};
}