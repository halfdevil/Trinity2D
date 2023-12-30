#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

namespace Trinity
{
	template <typename T>
	class ObjectPool
	{
	public:

		using Allocator = std::function<std::unique_ptr<T>(void)>;

		ObjectPool() = default;
		~ObjectPool()
		{
			destroy();
		}

		const std::vector<T*>& getObjects() const
		{
			return mObjects;
		}

		void create(uint32_t size, Allocator allocator)
		{
			mAllocator = allocator;
			resize(size);
		}

		void destroy()
		{
			mObjects.clear();
			mObjectArray.clear();
			mNumObjects = 0;
			mSize = 0;
		}

		uint32_t getSize() const
		{
			return mSize;
		}

		uint32_t getNumObjects() const
		{
			return mNumObjects;
		}

		T* allocate()
		{
			auto* object = mObjects[mNumObjects];
			if (mNumObjects == mSize - 1)
			{
				resize(mSize * 2);
			}

			mNumObjects++;
			return object;
		}

		void release(T* object)
		{
			if (mNumObjects > 0)
			{
				mObjects[--mNumObjects] = object;
			}
		}

		void resize(uint32_t newSize)
		{
			mObjectArray.resize(newSize);

			for (uint32_t idx = mSize; idx < newSize; idx++)
			{
				auto object = mAllocator();

				mObjects.push_back(object.get());
				mObjectArray.push_back(std::move(object));

				mSize = newSize;
			}
		}

	private:

		uint32_t mSize{ 0 };
		uint32_t mNumObjects{ 0 };
		Allocator mAllocator;
		std::vector<T*> mObjects;
		std::vector<std::unique_ptr<T>> mObjectArray;
	};
}