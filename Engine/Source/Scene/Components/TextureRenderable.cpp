#include "Scene/Components/TextureRenderable.h"
#include "Scene/Scene.h"
#include "Editor/EditorLayout.h"
#include "Graphics/Texture.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	std::type_index TextureRenderable::getType() const
	{
		return typeid(TextureRenderable);
	}

	UUIDv4::UUID TextureRenderable::getUUID() const
	{
		return TextureRenderable::UUID;
	}

	Editor* TextureRenderable::getEditor()
	{
		static TextureRenderableEditor editor;
		editor.setTextureRenderable(*this);

		return &editor;
	}

	Serializer* TextureRenderable::getSerializer(Scene& scene)
	{
		static TextureRenderableSerializer serializer;
		serializer.setTextureRenderable(*this);
		serializer.setScene(scene);

		return &serializer;
	}

	void TextureRenderable::setTexture(Texture& texture)
	{
		mTexture = &texture;
	}

	void TextureRenderable::setOrigin(const glm::vec2& origin)
	{
		mOrigin = origin;
	}

	void TextureRenderable::setColor(const glm::vec4& color)
	{
		mColor = color;
	}

	void TextureRenderable::setFlip(const glm::bvec2& flip)
	{
		mFlip = flip;
	}

	void TextureRenderableEditor::setTextureRenderable(TextureRenderable& renderable)
	{
		mTextureRenderable = &renderable;
	}

	void TextureRenderableEditor::onInspectorGui(const EditorLayout& layout)
	{
		if (layout.beginLayout("Texture Renderable"))
		{
			layout.inputVec2("Origin", mTextureRenderable->mOrigin);
			layout.inputVec4("Color", mTextureRenderable->mColor);
			layout.endLayout();
		}
	}

	void TextureRenderableSerializer::setTextureRenderable(TextureRenderable& renderable)
	{
		mTextureRenderable = &renderable;
		setComponent(renderable);
	}

	bool TextureRenderableSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		if (!ComponentSerializer::read(reader, cache))
		{
			LogError("ComponentSerialzer::read() failed");
			return false;
		}

		auto textureFileName = reader.readString();
		if (!textureFileName.empty())
		{
			auto texture = std::make_unique<Texture>();
			if (!texture->create(textureFileName, wgpu::TextureFormat::RGBA8Unorm))
			{
				LogError("Texture::create() failed for: '%s'", textureFileName.c_str());
				return false;
			}

			mTextureRenderable->mTexture = texture.get();
			cache.addResource(std::move(texture));
		}

		if (!reader.read(glm::value_ptr(mTextureRenderable->mOrigin)))
		{
			LogError("FileReader::read() failed for 'origin'");
			return false;
		}

		if (!reader.read(glm::value_ptr(mTextureRenderable->mColor)))
		{
			LogError("FileReader::read() failed for 'color'");
			return false;
		}

		if (!reader.read(glm::value_ptr(mTextureRenderable->mFlip)))
		{
			LogError("FileReader::read() failed for 'flip'");
			return false;
		}

		return true;
	}

	bool TextureRenderableSerializer::write(FileWriter& writer)
	{
		if (!ComponentSerializer::write(writer))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		std::string textureFileName;
		if (mTextureRenderable->mTexture != nullptr)
		{
			textureFileName = mTextureRenderable->mTexture->getFileName();
		}

		if (!writer.writeString(textureFileName))
		{
			LogError("FileWriter::writeString() failed for '%s'", textureFileName.c_str());
			return false;
;		}

		if (!writer.write(glm::value_ptr(mTextureRenderable->mOrigin)))
		{
			LogError("FileReader::write() failed for 'origin'");
			return false;
		}

		if (!writer.write(glm::value_ptr(mTextureRenderable->mColor)))
		{
			LogError("FileReader::write() failed for 'color'");
			return false;
		}

		if (!writer.write(glm::value_ptr(mTextureRenderable->mFlip)))
		{
			LogError("FileReader::write() failed for 'flip'");
			return false;
		}

		return true;
	}

	bool TextureRenderableSerializer::read(json& object, ResourceCache& cache)
	{
		if (!ComponentSerializer::read(object, cache))
		{
			LogError("ComponentSerialzer::read() failed");
			return false;
		}

		if (!object.contains("texture"))
		{
			LogError("TextureRenderable JSON object doesn't contains 'texture' key");
			return false;
		}

		if (!object.contains("origin"))
		{
			LogError("TextureRenderable JSON object doesn't contains 'origin' key");
			return false;
		}

		if (!object.contains("color"))
		{
			LogError("TextureRenderable JSON object doesn't contains 'color' key");
			return false;
		}

		if (!object.contains("flip"))
		{
			LogError("TextureRenderable JSON object doesn't contains 'flip' key");
			return false;
		}

		auto textureFileName = object["texture"].get<std::string>();
		if (!textureFileName.empty())
		{
			auto texture = std::make_unique<Texture>();
			if (!texture->create(textureFileName, wgpu::TextureFormat::RGBA8Unorm))
			{
				LogError("Texture::create() failed for: '%s'", textureFileName.c_str());
				return false;
			}

			mTextureRenderable->mTexture = texture.get();
			cache.addResource(std::move(texture));
		}

		mTextureRenderable->mOrigin = {
			object["origin"][0].get<float>(),
			object["origin"][1].get<float>()
		};

		mTextureRenderable->mColor = {
			object["color"][0].get<float>(),
			object["color"][1].get<float>(),
			object["color"][1].get<float>(),
			object["color"][1].get<float>()
		};

		mTextureRenderable->mFlip = {
			object["flip"][0].get<bool>(),
			object["flip"][1].get<bool>()
		};

		return true;
	}

	bool TextureRenderableSerializer::write(json& object)
	{
		if (!ComponentSerializer::write(object))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		std::string textureFileName;
		if (mTextureRenderable->mTexture != nullptr)
		{
			textureFileName = mTextureRenderable->mTexture->getFileName();
		}

		object["texture"] = textureFileName;
		object["origin"] = std::vector<float>{
			mTextureRenderable->mOrigin.x,
			mTextureRenderable->mOrigin.y
		};

		object["color"] = std::vector<float>{
			mTextureRenderable->mColor.x,
			mTextureRenderable->mColor.y,
			mTextureRenderable->mColor.z,
			mTextureRenderable->mColor.w
		};

		object["flip"] = std::vector<bool>{
			mTextureRenderable->mFlip.x,
			mTextureRenderable->mFlip.y
		};

		return true;
	}
}