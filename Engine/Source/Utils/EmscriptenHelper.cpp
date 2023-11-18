#include "Utils/EmscriptenHelper.h"

namespace Trinity
{
	std::unique_ptr<wgpu::ChainedStruct> EmscriptenHelper::getCanvasSurface(const std::string& selector, GLFWwindow* handle)
	{
		auto surfaceDesc = std::make_unique<wgpu::SurfaceDescriptorFromCanvasHTMLSelector>();
		surfaceDesc->selector = selector.c_str();
		return surfaceDesc;
	}
}