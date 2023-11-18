#include "Core/EditorResources.h"
#include "Core/ResourceCache.h"
#include "Graphics/Texture.h"
#include "ImGui/ImGuiFont.h"
#include "Core/Logger.h"
#include "VFS/FileSystem.h"
#include "IconsFontAwesome6.h"

namespace Trinity
{
	static constexpr ImWchar kIconFontRanges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };

	EditorResources::~EditorResources()
	{
		destroy();
	}

	bool EditorResources::create(json resourcesConfig)
	{
		mResourceCache = std::make_unique<ResourceCache>();
		if (resourcesConfig.contains("font"))
		{
			if (!loadFont(resourcesConfig["font"]))
			{
				LogError("EditorResources::loadFont() failed");
				return false;
			}
		}

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

	Texture* EditorResources::loadIcon(EditorIcon icon, uint32_t size)
	{
		return loadTexture(getIconPath(icon, size));
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

	bool EditorResources::loadFont(json fontConfig)
	{
		auto fontFile = fontConfig["file"].get<std::string>();
		auto fontSize = fontConfig["size"].get<float>();
		auto fontName = "editor";

		auto& fileSystem = FileSystem::get();
		auto fullPath = fileSystem.combinePath(kFontPath, fontFile);

		auto font = loadFont(fontName, fullPath, fontSize, false);
		if (!font)
		{
			LogError("EditorResource::loadFont() failed for: '%s'", fontFile.c_str());
			return false;
		}

		if (fontConfig.contains("icon"))
		{
			auto iconFontFile = fontConfig["icon"].get<std::string>();
			auto fullPath = fileSystem.combinePath(kFontPath, iconFontFile);

			if (!font->mergeFont(fullPath, kIconFontRanges))
			{
				LogError("ImGuiFont::mergeFont() failed for: '%s'", iconFontFile.c_str());
				return false;
			}
		}

		if (!font->build())
		{
			LogError("ImGuiFont::build() failed for: '%s'", fontFile.c_str());
			return false;
		}

		return true;
	}

	std::string EditorResources::getIconPath(EditorIcon icon, uint32_t size)
	{
		std::string fileName;

		switch(icon)
		{
		case EditorIcon::Folder:
			fileName = "folder.png";
			break;

		case EditorIcon::File:
			fileName = "file.png";

		default:
			break;
		}

		if (!fileName.empty())
		{
			auto& fileSystem = FileSystem::get();

			auto fullPath = fileSystem.combinePath(kIconPath, std::to_string(size));
			fullPath = fileSystem.combinePath(fullPath, fileName);

			return fullPath;
		}

		return {};
	}
}