#pragma once

#include <memory>
#include <string>

namespace Trinity
{
	class ResourceCache;
	class Texture;

	class EditorCache
	{
	public:

		EditorCache() = default;
		virtual ~EditorCache();

		EditorCache(const EditorCache&) = delete;
		EditorCache& operator = (const EditorCache&) = delete;

		EditorCache(EditorCache&&) = default;
		EditorCache& operator = (EditorCache&&) = default;

		ResourceCache* getResourceCache() const
		{
			return mResourceCache.get();
		}

		virtual bool create();
		virtual void destroy();

		virtual Texture* loadTexture(const std::string& fileName);

	protected:

		std::unique_ptr<ResourceCache> mResourceCache{ nullptr };
	};
}