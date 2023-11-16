#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "imgui.h"

namespace Trinity
{
	class ResourceCache;
	class Texture;
	class ImGuiFont;

	class EditorResources
	{
	public:

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

		virtual bool create();
		virtual void destroy();

		virtual Texture* getTexture(const std::string& fileName);
		virtual ImGuiFont* getFont(const std::string& name);

		virtual Texture* loadTexture(const std::string& fileName);
		virtual ImGuiFont* loadFont(const std::string& name, const std::string& filePath, float size = 20.0f,
			bool build = true, const ImWchar* glyphRanges = nullptr);
			 
	protected:

		std::unique_ptr<ResourceCache> mResourceCache{ nullptr };
		std::unordered_map<std::string, ImGuiFont*> mFonts;
	};
}