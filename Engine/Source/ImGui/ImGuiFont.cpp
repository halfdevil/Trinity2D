#include "ImGui/ImGuiFont.h"
#include "Graphics/Texture.h"
#include "VFS/FileSystem.h"
#include "Core/Logger.h"
#include "Core/Image.h"

namespace Trinity
{
	ImGuiFont::~ImGuiFont()
	{
		destroy();
	}

	bool ImGuiFont::create(const std::string& name, const std::string& filePath, float size, const ImWchar* glyphRanges)
	{
		auto file = FileSystem::get().openFile(filePath, FileOpenMode::OpenRead);
		if (!file)
		{
			LogError("Error opening font file: %s", filePath.c_str());
			return false;
		}

		FileReader reader(*file);
		std::vector<uint8_t> buffer(reader.getSize());
		reader.read(buffer.data(), reader.getSize());

		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig cfg;
		cfg.FontDataOwnedByAtlas = false;
		cfg.RasterizerMultiply = 1.5f;
		cfg.SizePixels = size;
		cfg.PixelSnapH = true;
		cfg.OversampleH = 4;
		cfg.OversampleV = 4;

		mHandle = io.Fonts->AddFontFromMemoryTTF(buffer.data(), (int)reader.getSize(),
			cfg.SizePixels, &cfg, glyphRanges);

		if (!mHandle)
		{
			LogError("ImGui::FontAtlas::AddFontFromMemoryTTF() failed for: %s!!", filePath.c_str());
			return false;
		}

		mName = name;
		mSize = size;

		return true;
	}

	void ImGuiFont::destroy()
	{
		mTexture = nullptr;
		mHandle = nullptr;
	}

	bool ImGuiFont::mergeFont(const std::string& filePath, const ImWchar* glyphRanges)
	{
		auto file = FileSystem::get().openFile(filePath, FileOpenMode::OpenRead);
		if (!file)
		{
			LogError("Error opening icons font file: %s", filePath.c_str());
			return false;
		}

		FileReader reader(*file);
		std::vector<uint8_t> buffer(reader.getSize());
		reader.read(buffer.data(), reader.getSize());

		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig cfg;
		cfg.MergeMode = true;
		cfg.FontDataOwnedByAtlas = false;
		cfg.GlyphMinAdvanceX = mSize;
		cfg.PixelSnapH = true;

		ImFont* iconsFont = io.Fonts->AddFontFromMemoryTTF(buffer.data(), (int)reader.getSize(),
			mSize, &cfg, glyphRanges);

		if (!iconsFont)
		{
			LogError("ImGui::FontAtlas::AddFontFromMemoryTTF() failed for: %s!!", filePath.c_str());
			return false;
		}

		if (!build())
		{
			LogError("ImGuiFont::build() failed for: '%s'", filePath.c_str());
			return false;
		}

		return true;
	}

	bool ImGuiFont::build()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (!io.Fonts->Build())
		{
			LogError("ImFontAtlas::Build() failed!!");
			return false;
		}

		uint8_t* pixels{ nullptr };
		int32_t width{ 0 }, height{ 0 };
		int32_t sizePerPixel{ 0 };
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &sizePerPixel);

		const wgpu::TextureUsage usage = wgpu::TextureUsage::TextureBinding |
			wgpu::TextureUsage::CopyDst;

		mTexture = std::make_unique<Texture>();
		if (!mTexture->create(width, height, wgpu::TextureFormat::RGBA8Unorm, usage))
		{
			LogError("Texture2D::create() failed!!");
			return false;
		}

		mTexture->upload(sizePerPixel, pixels, sizePerPixel * width * height);
		io.Fonts->SetTexID((ImTextureID)mTexture.get());

		return true;
	}

	void ImGuiFont::activate()
	{
		ImGui::PushFont(mHandle);
	}

	void ImGuiFont::deactivate()
	{
		ImGui::PopFont();
	}

	std::type_index ImGuiFont::getType() const
	{
		return typeid(ImGuiFont);
	}
}