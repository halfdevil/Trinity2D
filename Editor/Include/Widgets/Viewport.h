#pragma once

#include "Core/EditorWidget.h"
#include <stdint.h>
#include <memory>

namespace Trinity
{
	class FrameBuffer;
	class Texture;
	class EditorResources;

	class Viewport : public EditorWidget
	{
	public:

		Viewport() = default;
		virtual ~Viewport();

		Viewport(const Viewport&) = delete;
		Viewport& operator = (const Viewport&) = delete;

		Viewport(Viewport&&) = default;
		Viewport& operator = (Viewport&&) = default;

		FrameBuffer* getFrameBuffer() const
		{
			return mFrameBuffer.get();
		}

		virtual bool create(uint32_t width, uint32_t height, EditorResources& resources);
		virtual void destroy();

		virtual void resize(uint32_t width, uint32_t height);
		virtual void draw() override;

	protected:

		std::unique_ptr<FrameBuffer> mFrameBuffer{ nullptr };
	};
}