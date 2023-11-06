#pragma once

#include "Graphics/Buffer.h"

namespace Trinity
{
	class StorageBuffer : public Buffer
	{
	public:

		StorageBuffer() = default;
		~StorageBuffer();

		StorageBuffer(const StorageBuffer&) = delete;
		StorageBuffer& operator = (const StorageBuffer&) = delete;

		StorageBuffer(StorageBuffer&&) = default;
		StorageBuffer& operator = (StorageBuffer&&) = default;

		uint32_t getSize() const
		{
			return mSize;
		}

		virtual bool create(uint32_t size, const void* data = nullptr);
		virtual void destroy();

	protected:

		uint32_t mSize{ 0 };
	};
}