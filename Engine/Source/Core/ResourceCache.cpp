#include "Core/ResourceCache.h"
#include "Core/Resource.h"

namespace Trinity
{
	bool ResourceCache::hasResource(const std::type_index& type) const
	{
		auto it = mResources.find(type);
		return (it != mResources.end() && !it->second.empty());
	}

	bool ResourceCache::isLoaded(const std::type_index& type, const std::string& fileName) const
	{		
		if (auto it = mFileResourceIndexMap.find(type); it != mFileResourceIndexMap.end())
		{
			auto& indices = it->second;
			if (auto it2 = indices.find(fileName); it2 != indices.end())
			{
				return true;
			}
		}

		return false;
	}

	Resource* ResourceCache::getResource(const std::type_index& type, const std::string& fileName) const
	{		
		if (auto it = mFileResourceIndexMap.find(type); it != mFileResourceIndexMap.end())
		{
			auto& indices = it->second;
			auto& resources = mResources.at(type);

			if (auto it2 = indices.find(fileName); it2 != indices.end())
			{
				return resources[it2->second].get();
			}
		}

		return nullptr;
	}

	const std::vector<std::unique_ptr<Resource>>& ResourceCache::getResources(const std::type_index& type) const
	{
		return mResources.at(type);
	}

	void ResourceCache::addResource(std::unique_ptr<Resource> resource)
	{
		auto& resources = mResources[resource->getType()];
		auto& indices = mFileResourceIndexMap[resource->getType()];

		if (!resource->getFileName().empty())
		{
			indices.insert(std::make_pair(resource->getFileName(), (uint32_t)resources.size()));
		}

		resources.push_back(std::move(resource));
	}

	void ResourceCache::setResources(const std::type_index& type, std::vector<std::unique_ptr<Resource>> resources)
	{
		for (auto& resource : resources)
		{
			addResource(std::move(resource));
		}
	}

	void ResourceCache::removeResource(const std::type_index& type, const std::string& fileName)
	{
		if (mResources.contains(type))
		{
			auto& indices = mFileResourceIndexMap[type];
			auto& resources = mResources[type];

			if (auto it = indices.find(fileName); it != indices.end())
			{
				resources.erase(resources.begin() + it->second);
				indices.erase(it);
			}
		}
	}

	void ResourceCache::removeResource(Resource* resource)
	{
		if (auto it = mResources.find(resource->getType()); it != mResources.end())
		{
			auto& resources = it->second;
			for (uint32_t idx = 0; idx < (uint32_t)resources.size(); idx++)
			{
				auto& res = resources[idx];
				if (res.get() == resource)
				{
					resources.erase(resources.begin() + idx);
				}
			}
		}
	}

	void ResourceCache::clearResources(const std::type_index& type)
	{
		mResources.erase(type);
		mFileResourceIndexMap.erase(type);
	}

	void ResourceCache::clear()
	{
		mResources.clear();
		mFileResourceIndexMap.clear();
	}
}