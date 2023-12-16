#include "TileMap/TileSet.h"
#include "Editor/EditorLayout.h"
#include "Graphics/Texture.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"

namespace Trinity
{
	glm::vec2 TileSet::getPosition(uint32_t globalId) const
	{
		auto localId = globalId - mFirstId;
		auto tileX = localId % mNumTiles.x;
		auto tileY = (mNumTiles.y - 1) - localId / mNumTiles.x;
		auto position = glm::vec2{ tileX * mTileSize.x, tileY * mTileSize.y };

		if (mSpacing > 0.0f)
		{
			position.x += mSpacing + (mSpacing * (localId % mNumTiles.x));
			position.y += mSpacing + (mSpacing * ((mNumTiles.y - 1) - localId / mNumTiles.x));
		}

		return position;
	}

	IEditor* TileSet::getEditor()
	{
		static TileSetEditor editor;
		editor.setTileSet(*this);

		return &editor;
	}

	ISerializer* TileSet::getSerializer()
	{
		static TileSetSerializer serializer;
		serializer.setTileSet(*this);

		return &serializer;
	}

	void TileSet::setName(const std::string& name)
	{
		mName = name;
	}

	void TileSet::setSize(const glm::vec2& size)
	{
		mSize = size;
	}

	void TileSet::setTileSize(const glm::vec2& tileSize)
	{
		mTileSize = tileSize;
	}

	void TileSet::setNumTiles(const glm::uvec2& numTiles)
	{
		mNumTiles = numTiles;
	}

	void TileSet::setSpacing(float spacing)
	{
		mSpacing = spacing;
	}

	void TileSet::setFirstId(uint32_t id)
	{
		mFirstId = id;
	}

	void TileSet::setTexture(Texture* texture)
	{
		mTexture = texture;
	}

	void TileSet::addProperty(uint32_t id, const TileProperty& property)
	{
		if (auto it = mProperties.find(id); it != mProperties.end())
		{
			it->second.push_back(property);
		}
		else
		{
			std::vector<TileProperty> properties{ property };
			mProperties.insert(std::make_pair(id, std::move(properties)));
		}
	}

	void TileSet::setProperties(uint32_t id, std::vector<TileProperty>&& properties)
	{
		if (auto it = mProperties.find(id); it != mProperties.end())
		{
			it->second = std::move(properties);
		}
		else
		{
			mProperties.insert(std::make_pair(id, std::move(properties)));
		}
	}

	void TileSetEditor::setTileSet(TileSet& tileSet)
	{
		mTileSet = &tileSet;

		if (mTileSet->getTexture() != nullptr)
		{
			mSelectedTextureFile = mTileSet->getTexture()->getFileName();
		}
		else
		{
			mSelectedTextureFile.clear();
		}
	}

	void TileSetEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{
		if (layout.beginLayout("Tile Set"))
		{
			layout.inputString("Name", mTileSet->mName);
			layout.inputVec2("Tile Size", mTileSet->mTileSize);
			layout.inputSize("No. of Tiles", mTileSet->mNumTiles);
			layout.inputFloat("Spacing", mTileSet->mSpacing);

			if (layout.fileCombo("Texture", FileType::Texture, mSelectedTextureFile))
			{
				if (!mTileSet->mTexture || mTileSet->mTexture->getFileName() != mSelectedTextureFile)
				{
					if (!cache.isLoaded<Texture>(mSelectedTextureFile))
					{
						auto texture = std::make_unique<Texture>();
						if (!texture->create(mSelectedTextureFile, wgpu::TextureFormat::RGBA8Unorm))
						{
							LogError("Texture::create() failed for: '%s'", mSelectedTextureFile.c_str());
						}
						else
						{
							cache.addResource(std::move(texture));
						}
					}

					mTileSet->mTexture = cache.getResource<Texture>(mSelectedTextureFile);
				}
			}

			layout.endLayout();
		}
	}

	void TileSetSerializer::setTileSet(TileSet& tileSet)
	{
		mTileSet = &tileSet;
	}

	bool TileSetSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		mTileSet->mName = reader.readString();

		if (!reader.read(&mTileSet->mTileSize))
		{
			LogError("FileReader::read() failed for 'tile size'");
			return false;
		}

		if (!reader.read(&mTileSet->mNumTiles))
		{
			LogError("FileReader::read() failed for 'num tiles'");
			return false;
		}

		if (!reader.read(&mTileSet->mSpacing))
		{
			LogError("FileReader::read() failed for 'spacing'");
			return false;
		}

		if (!reader.read(&mTileSet->mFirstId))
		{
			LogError("FileReader::read() failed for 'first id'");
			return false;
		}

		auto textureFileName = reader.readString();
		if (!textureFileName.empty())
		{
			if (!cache.isLoaded<Texture>(textureFileName))
			{
				auto texture = std::make_unique<Texture>();
				if (!texture->create(textureFileName, wgpu::TextureFormat::RGBA8Unorm))
				{
					LogError("Texture::create() failed for: '%s'", textureFileName.c_str());
					return false;
				}
				else
				{
					cache.addResource(std::move(texture));
				}
			}

			mTileSet->mTexture = cache.getResource<Texture>(textureFileName);
			mTileSet->mSize = glm::vec2{ 
				(float)mTileSet->mTexture->getWidth(), 
				(float)mTileSet->mTexture->getHeight() 
			};
		}

		uint32_t numProperties{ 0 };
		if (!reader.read(&numProperties))
		{
			LogError("FileReader::read() failed for 'num properties'");
			return false;
		}

		if (numProperties > 0)
		{
			std::vector<uint32_t> keys(numProperties);
			std::vector<std::vector<TileProperty>> values(numProperties);

			if (!reader.read(keys.data(), numProperties))
			{
				LogError("FileReader::read() failed for 'properties keys'");
				return false;
			}

			for (uint32_t idx = 0; idx < numProperties; idx++)
			{
				uint32_t numTileProperties{ 0 };
				if (!reader.read(&numTileProperties))
				{
					LogError("FileReader::read() failed for 'num tile properties'");
					return false;
				}

				if (numTileProperties > 0)
				{
					std::vector<TileProperty> properties;
					for (uint32_t jdx = 0; jdx < numTileProperties; jdx++)
					{
						auto key = reader.readString();
						auto value = reader.readString();

						properties.push_back({ key, value });
					}

					values[idx] = std::move(properties);
				}

				for (uint32_t idx = 0; idx < numProperties; idx++)
				{
					mTileSet->mProperties.insert(std::make_pair(keys[idx], std::move(values[idx])));
				}
			}
		}

		return true;
	}

	bool TileSetSerializer::write(FileWriter& writer)
	{
		if (!writer.writeString(mTileSet->mName))
		{
			LogError("FileWriter::writeString() failed for 'name'");
			return false;
		}

		if (!writer.write(&mTileSet->mTileSize))
		{
			LogError("FileWriter::write() failed for 'tile size'");
			return false;
		}

		if (!writer.write(&mTileSet->mNumTiles))
		{
			LogError("FileWriter::write() failed for 'num tiles'");
			return false;
		}

		if (!writer.write(&mTileSet->mSpacing))
		{
			LogError("FileWriter::write() failed for 'spacing'");
			return false;
		}

		if (!writer.write(&mTileSet->mFirstId))
		{
			LogError("FileWriter::write() failed for 'first id'");
			return false;
		}

		std::string textureFileName;
		if (mTileSet->mTexture != nullptr)
		{
			textureFileName = mTileSet->mTexture->getFileName();
		}

		if (!writer.writeString(textureFileName))
		{
			LogError("FileWriter::writeString() failed for '%s'", textureFileName.c_str());
			return false;
		}

		const auto numProperties = (uint32_t)mTileSet->mProperties.size();
		if (!writer.write(&numProperties))
		{
			LogError("FileWriter::write() failed for 'num properties'");
			return false;
		}

		if (numProperties > 0)
		{
			std::vector<uint32_t> keys;
			std::vector<std::vector<TileProperty>> values;

			keys.reserve(numProperties);
			values.reserve(numProperties);

			for (auto& [key, value] : mTileSet->mProperties)
			{
				keys.push_back(key);
				values.push_back(value);
			}

			if (!writer.writeVector(keys))
			{
				LogError("FileWriter::write() failed for 'properties keys'");
				return false;
			}

			for (auto& value : values)
			{
				const auto numTileProperties = (uint32_t)value.size();
				if (!writer.write(&numTileProperties))
				{
					LogError("FileWriter::write() failed for 'num tile properties'");
					return false;
				}

				if (numTileProperties > 0)
				{
					for (auto& tileProperty : value)
					{
						if (!writer.writeString(tileProperty.name))
						{
							LogError("FileWriter::write() failed for 'tile property name'");
							return false;
						}

						if (!writer.writeString(tileProperty.value))
						{
							LogError("FileWriter::write() failed for 'tile property value'");
							return false;
						}
					}
				}
			}
		}

		return true;
	}

	bool TileSetSerializer::read(json& object, ResourceCache& cache)
	{
		if (!object.contains("name"))
		{
			LogError("JSON TileSet object doesn't have 'name' key");
			return false;
		}

		if (!object.contains("tileSize"))
		{
			LogError("JSON TileSet object doesn't have 'tile size' key");
			return false;
		}

		if (!object.contains("numTiles"))
		{
			LogError("JSON TileSet object doesn't have 'num tiles' key");
			return false;
		}

		if (!object.contains("spacing"))
		{
			LogError("JSON TileSet object doesn't have 'spacing' key");
			return false;
		}

		if (!object.contains("firstId"))
		{
			LogError("JSON TileSet object doesn't have 'first id' key");
			return false;
		}

		if (!object.contains("texture"))
		{
			LogError("JSON TileSet object doesn't have 'texture' key");
			return false;
		}

		if (!object.contains("properties"))
		{
			LogError("JSON TileSet object doesn't have 'properties' key");
			return false;
		}

		mTileSet->mName = object["name"].get<std::string>();
		mTileSet->mTileSize = {
			object["tileSize"][0].get<float>(),
			object["tileSize"][1].get<float>()
		};

		mTileSet->mNumTiles = {
			object["numTiles"][0].get<uint32_t>(),
			object["numTiles"][1].get<uint32_t>()
		};

		mTileSet->mSpacing = object["spacing"].get<float>();
		mTileSet->mFirstId = object["firstId"].get<uint32_t>();

		auto textureFileName = object["texture"].get<std::string>();
		if (!textureFileName.empty())
		{
			if (!cache.isLoaded<Texture>(textureFileName))
			{
				auto texture = std::make_unique<Texture>();
				if (!texture->create(textureFileName, wgpu::TextureFormat::RGBA8Unorm))
				{
					LogError("Texture::create() failed for: '%s'", textureFileName.c_str());
					return false;
				}
				else
				{
					cache.addResource(std::move(texture));
				}
			}

			mTileSet->mTexture = cache.getResource<Texture>(textureFileName);
			mTileSet->mSize = glm::vec2{
				(float)mTileSet->mTexture->getWidth(),
				(float)mTileSet->mTexture->getHeight()
			};
		}

		for (auto& propertyJson : object["properties"])
		{
			std::vector<TileProperty> properties;
			for (auto& tilePropertiesJson : propertyJson["tileProperties"])
			{
				properties.push_back({
					tilePropertiesJson["name"].get<std::string>(),
					tilePropertiesJson["value"].get<std::string>()
				});
			}

			auto tileId = propertyJson["tileId"].get<uint32_t>();
			mTileSet->mProperties.insert(std::make_pair(tileId, std::move(properties)));
		}
		
		return true;
	}

	bool TileSetSerializer::write(json& object)
	{
		std::string textureFileName;
		if (mTileSet->mTexture != nullptr)
		{
			textureFileName = mTileSet->mTexture->getFileName();
		}

		json propertiesJson;
		for (auto& [key, value] : mTileSet->mProperties)
		{
			json tilePropertiesJson;
			for (auto& property : value)
			{
				json propertyJson;
				propertyJson[property.name] = property.value;

				tilePropertiesJson.push_back(std::move(propertyJson));
			}

			propertiesJson["tileId"] = key;
			propertiesJson["tileProperties"] = std::move(tilePropertiesJson);
		}

		object["name"] = mTileSet->mName;
		object["tileSize"] = std::vector<float>{
			mTileSet->mTileSize.x,
			mTileSet->mTileSize.y
		};

		object["numTiles"] = std::vector<uint32_t>{
			mTileSet->mNumTiles.x,
			mTileSet->mNumTiles.y
		};

		object["spacing"] = mTileSet->mSpacing;
		object["firstId"] = mTileSet->mFirstId;
		object["texture"] = textureFileName;
		object["properties"] = std::move(propertiesJson);

		return true;
	}
}