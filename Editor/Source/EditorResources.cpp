#include "EditorResources.h"
#include "Core/ResourceCache.h"
#include "Graphics/Texture.h"
#include "ImGui/ImGuiFont.h"
#include "Core/Logger.h"

namespace Trinity
{
	EditorResources::~EditorResources()
	{
		destroy();
	}

	bool EditorResources::create()
	{
		mResourceCache = std::make_unique<ResourceCache>();
		return true;
	}

	void EditorResources::destroy()
	{
		mResourceCache->clear();
		mResourceCache = nullptr;
	}

	Texture* EditorResources::getTexture(const std::string& fileName)
	{
		return mResourceCache->getResource<Texture>(fileName);
	}

	ImGuiFont* EditorResources::getFont(const std::string& name)
	{
		if (auto it = mFonts.find(name); it != mFonts.end())
		{
			return it->second;
		}

		return nullptr;
	}

	Texture* EditorResources::loadTexture(const std::string& fileName)
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

	ImGuiFont* EditorResources::loadFont(const std::string& name, const std::string& filePath, 
		float size, bool build, const ImWchar* glyphRanges)
	{
		if (auto it = mFonts.find(name); it == mFonts.end())
		{
			auto font = std::make_unique<ImGuiFont>();
			if (!font->create(name, filePath, size, glyphRanges))
			{
				LogError("ImGuiFont::create() failed for: '%s'", filePath.c_str());
				return nullptr;
			}

			if (build)
			{
				if (!font->build())
				{
					LogError("ImGuiFont::build() failed for: '%s'", filePath.c_str());
					return nullptr;
				}
			}

			mFonts.insert(std::make_pair(name, font.get()));
			mResourceCache->addResource(std::move(font));
		}

		return mFonts.at(name);
	}
}