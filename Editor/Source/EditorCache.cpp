#include "EditorCache.h"
#include "Core/ResourceCache.h"
#include "Graphics/Texture.h"
#include "Core/Logger.h"

namespace Trinity
{
	EditorCache::~EditorCache()
	{
		destroy();
	}

	bool EditorCache::create()
	{
		mResourceCache = std::make_unique<ResourceCache>();
		return true;
	}

	void EditorCache::destroy()
	{
		mResourceCache->clear();
		mResourceCache = nullptr;
	}

	Texture* EditorCache::loadTexture(const std::string& fileName)
	{
		if (!mResourceCache->isLoaded<Texture>(fileName))
		{
			auto texture = std::make_unique<Texture>();
			if (!texture->create(fileName, wgpu::TextureFormat::RGBA8Unorm))
			{
				LogError("Texture::create() failed for: '%s'", fileName.c_str());
				return nullptr;
			}

			mResourceCache->addResource(std::move(texture));
		}

		return mResourceCache->getResource<Texture>(fileName);
	}
}