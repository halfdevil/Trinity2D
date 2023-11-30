#include "Gui/Widgets/Panel.h"
#include "Gui/GuiRenderer.h"
#include "Graphics/Texture.h"
#include "Editor/EditorLayout.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	IEditor* Panel::getEditor(Gui& gui)
	{
		static PanelEditor editor;
		editor.setPanel(*this);
		editor.setGui(gui);

		return &editor;
	}

	ISerializer* Panel::getSerializer(Gui& gui)
	{
		static PanelSerializer serializer;
		serializer.setPanel(*this);
		serializer.setGui(gui);

		return &serializer;
	}

	void Panel::setBackgroundTexture(Texture& texture)
	{
		mBackgroundTexture = &texture;
	}

	void Panel::setBackgroundColor(const glm::vec4& color)
	{
		mBackgroundColor = color;
	}

	std::type_index Panel::getType() const
	{
		return typeid(Panel);
	}

	UUIDv4::UUID Panel::getTypeUUID() const
	{
		return Panel::UUID;
	}

	void Panel::preDraw(GuiRenderer& renderer)
	{
		if (mBackgroundTexture != nullptr)
		{
			auto srcSize = glm::vec2{ 
				mBackgroundTexture->getWidth(), 
				mBackgroundTexture->getHeight() 
			};

			renderer.drawTexture(
				mBackgroundTexture, 
				glm::vec2{ 0.0f },
				srcSize,
				mScreenPosition,
				mScreenSize,
				glm::vec2{ 0.0f },
				mTransform.getWorldMatrix()
			);
		}
		else
		{
			renderer.drawRect(
				mScreenPosition,
				mScreenSize,
				glm::vec2{ 0.0f },
				mTransform.getWorldMatrix(),
				mBackgroundColor
			);
		}
	}

	void Panel::postDraw(GuiRenderer& renderer)
	{
	}

	void PanelEditor::setPanel(Panel& panel)
	{
		mPanel = &panel;
		setWidget(panel);
	}

	void PanelEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{
		addCommonFields(layout, cache);
		addTransform(layout, cache);

		if (layout.beginLayout("Panel"))
		{
			if (layout.fileCombo("Background Texture", FileType::Texture, mSelectedTextureFile))
			{
				if (!mPanel->mBackgroundTexture || mPanel->mBackgroundTexture->getFileName() != mSelectedTextureFile)
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

					mPanel->mBackgroundTexture = cache.getResource<Texture>(mSelectedTextureFile);
				}
			}

			layout.inputVec4("Background Color", mPanel->mBackgroundColor);
			layout.endLayout();
		}
	}

	void PanelSerializer::setPanel(Panel& panel)
	{
		mPanel = &panel;
		setWidget(panel);
	}

	bool PanelSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		if (!WidgetSerializer::read(reader, cache))
		{
			LogError("WidgetSerializer::read() failed");
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

			mPanel->mBackgroundTexture = texture.get();
			cache.addResource(std::move(texture));
		}

		if (!reader.read(glm::value_ptr(mPanel->mBackgroundColor)))
		{
			LogError("FileReader::read() failed for 'background color'");
			return false;
		}

		return true;
	}

	bool PanelSerializer::write(FileWriter& writer)
	{
		if (!WidgetSerializer::write(writer))
		{
			LogError("WidgetSerializer::write() failed");
			return false;
		}

		std::string textureFileName;
		if (mPanel->mBackgroundTexture != nullptr)
		{
			textureFileName = mPanel->mBackgroundTexture->getFileName();
		}

		if (!writer.writeString(textureFileName))
		{
			LogError("FileWriter::writeString() failed for '%s'", textureFileName.c_str());
			return false;
		}

		if (!writer.write(glm::value_ptr(mPanel->mBackgroundColor)))
		{
			LogError("FileReader::write() failed for 'color'");
			return false;
		}

		return true;
	}

	bool PanelSerializer::read(json& object, ResourceCache& cache)
	{
		if (!WidgetSerializer::read(object, cache))
		{
			LogError("WidgetSerializer::read() failed");
			return false;
		}

		if (!object.contains("backgroundTexture"))
		{
			LogError("Panel JSON object doesn't contains 'background texture' key");
			return false;
		}

		if (!object.contains("backgroundColor"))
		{
			LogError("Panel JSON object doesn't contains 'background color' key");
			return false;
		}

		auto textureFileName = object["backgroundTexture"].get<std::string>();
		if (!textureFileName.empty())
		{
			auto texture = std::make_unique<Texture>();
			if (!texture->create(textureFileName, wgpu::TextureFormat::RGBA8Unorm))
			{
				LogError("Texture::create() failed for: '%s'", textureFileName.c_str());
				return false;
			}

			mPanel->mBackgroundTexture = texture.get();
			cache.addResource(std::move(texture));
		}

		mPanel->mBackgroundColor = {
			object["backgroundColor"][0].get<float>(),
			object["backgroundColor"][1].get<float>(),
			object["backgroundColor"][2].get<float>(),
			object["backgroundColor"][3].get<float>()
		};

		return true;
	}

	bool PanelSerializer::write(json& object)
	{
		if (!WidgetSerializer::write(object))
		{
			LogError("WidgetSerializer::write() failed");
			return false;
		}

		std::string textureFileName;
		if (mPanel->mBackgroundTexture != nullptr)
		{
			textureFileName = mPanel->mBackgroundTexture->getFileName();
		}

		object["backgroundTexture"] = textureFileName;
		object["backgroundColor"] = std::vector<float>{
			mPanel->mBackgroundColor.x,
			mPanel->mBackgroundColor.y,
			mPanel->mBackgroundColor.z,
			mPanel->mBackgroundColor.w
		};

		return true;
	}
}