#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "imgui.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace Trinity
{
	class ResourceCache;
	class Texture;
	class ImGuiFont;

	enum class EditorIcon
	{
		Folder,
		File,
		Error,
		Warning,
		Info
	};

	class EditorResources
	{
	public:

		static constexpr const char* kFontPath = "/Assets/Editor/Fonts";
		static constexpr const char* kIconPath = "/Assets/Editor/Icons";

		EditorResources() = default;
		virtual ~EditorResources();

		EditorResources(const EditorResources&) = delete;
		EditorResources& operator = (const EditorResources&) = delete;

		EditorResources(EditorResources&&) = default;
		EditorResources& operator = (EditorResources&&) = default;

		ResourceCache* getResourceCache() const
		{
			return mResourceCache.get();
		}

		virtual bool create(json resourcesConfig);
		virtual void destroy();

		virtual Texture* getTexture(const std::string& fileName);
		virtual ImGuiFont* getFont(const std::string& name);

		virtual Texture* loadTexture(const std::string& fileName);
		virtual Texture* loadIcon(EditorIcon icon, uint32_t size);

		virtual ImGuiFont* loadFont(const std::string& name, const std::string& filePath, float size = 20.0f,
			bool build = true, const ImWchar* glyphRanges = nullptr);

	protected:

		virtual bool loadFont(json fontConfig);
		virtual std::string getIconPath(EditorIcon icon, uint32_t size);
			 
	protected:

		std::unique_ptr<ResourceCache> mResourceCache{ nullptr };
		std::unordered_map<std::string, ImGuiFont*> mFonts;
	};
}