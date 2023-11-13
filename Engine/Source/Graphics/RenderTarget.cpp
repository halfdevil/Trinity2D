#include "Graphics/RenderTarget.h"

namespace Trinity
{
	RenderTarget::~RenderTarget()
	{
	}

	std::type_index RenderTarget::getType() const
	{
		return typeid(RenderTarget);
	}
}