#pragma once

#include "Core/Resource.h"
#include <string>
#include <memory>
#include <unordered_map>
#include "imgui.h"

namespace Trinity
{
	class Texture;
	class Sampler;
	class BindGroup;
	class BindGroupLayout;

	class ImGuiFont : public Resource
	{
	public:

		ImGuiFont() = default;
		~ImGuiFont();

		ImGuiFont(const ImGuiFont&) = delete;
		ImGuiFont& operator = (const ImGuiFont&) = delete;

		ImGuiFont(ImGuiFont&&) noexcept = default;
		ImGuiFont& operator = (ImGuiFont&&) noexcept = default;

		const std::string& getName() const
		{
			return mName;
		}

		float getSize() const
		{
			return mSize;
		}

		ImFont* getHandle() const
		{
			return mHandle;
		}

		Texture* getTexture() const
		{
			return mTexture.get();
		}

		virtual bool create(const std::string& name, const std::string& filePath, float size = 20.0f, 
			const ImWchar* glyphRanges = nullptr);

		virtual void destroy();
		virtual bool mergeFont(const std::string& filePath, const ImWchar* glyphRanges = nullptr);
		virtual bool build();

		virtual void activate();
		virtual void deactivate();

		virtual std::type_index getType() const override;

	protected:

		std::string mName;
		float mSize{ 0.0f };
		ImFont* mHandle{ nullptr };
		std::unique_ptr<Texture> mTexture{ nullptr };
	};
}