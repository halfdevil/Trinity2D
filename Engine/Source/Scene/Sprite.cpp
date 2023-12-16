#include "Scene/Sprite.h"
#include "Editor/EditorLayout.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	Sprite::Sprite()
	{
		mFrames.reserve(kMaxFrames);
		mAnimations.reserve(kMaxAnimations);
	}

	std::type_index Sprite::getType() const
	{
		return typeid(Sprite);
	}

	IEditor* Sprite::getEditor(uint32_t selectedFrame, uint32_t selectedAnimation)
	{
		static SpriteEditor editor;
		editor.setSprite(*this);
		editor.setSelectedFrame(selectedFrame);
		editor.setSelectedAnimation(selectedAnimation);

		return &editor;
	}

	ISerializer* Sprite::getSerializer()
	{
		static SpriteSerializer serializer;
		serializer.setSprite(*this);

		return &serializer;
	}

	SpriteFrame* Sprite::getFrame(uint32_t idx)
	{
		if (idx < (uint32_t)mFrames.size())
		{
			return &mFrames[idx];
		}

		return nullptr;
	}

	SpriteAnimation* Sprite::getAnimation(uint32_t idx)
	{
		if (idx < (uint32_t)mAnimations.size())
		{
			return &mAnimations[idx];
		}

		return nullptr;
	}

	SpriteAnimation* Sprite::getAnimation(const std::string& name)
	{
		for (auto& animation : mAnimations)
		{
			if (animation.name == name)
			{
				return &animation;
			}
		}

		return nullptr;
	}

	void Sprite::setSize(const glm::vec2& size)
	{
		mSize = size;
	}

	void Sprite::setTexture(Texture& texture)
	{
		mTexture = &texture;
	}

	void Sprite::setFrames(std::vector<SpriteFrame>&& frames)
	{
		mFrames = std::move(frames);
	}

	void Sprite::setAnimations(std::vector<SpriteAnimation>&& animations)
	{
		mAnimations = std::move(animations);
	}

	void Sprite::addFrame(SpriteFrame&& frame)
	{
		mFrames.push_back(std::move(frame));
	}

	void Sprite::addAnimation(SpriteAnimation&& animation)
	{
		mAnimations.push_back(std::move(animation));
	}

	void Sprite::removeFrame(uint32_t frameIndex)
	{
		if (frameIndex < (uint32_t)mFrames.size())
		{
			mFrames.erase(mFrames.begin() + frameIndex);
		}
	}

	void Sprite::removeAnimation(uint32_t animationIndex)
	{
		if (animationIndex < (uint32_t)mAnimations.size())
		{
			mAnimations.erase(mAnimations.begin() + animationIndex);
		}
	}

	void Sprite::moveFrame(uint32_t from, uint32_t to)
	{
		if (from < (uint32_t)mFrames.size() && to < (uint32_t)mFrames.size())
		{
			std::swap(mFrames[from], mFrames[to]);
		}
	}

	void Sprite::draw(BatchRenderer& batchRenderer,	uint32_t frameIndex, const glm::vec2& origin, const glm::mat4& transform,
		const glm::vec4& color,	bool flipX,	bool flipY)
	{
		auto* frame = getFrame(frameIndex);
		if (frame != nullptr && mTexture != nullptr)
		{
			batchRenderer.drawTexture(
				mTexture,
				frame->position,
				frame->size,
				origin,
				transform,
				color,
				flipX,
				flipY
			);
		}
	}

	void SpriteEditor::setSprite(Sprite& sprite)
	{
		if (mSprite != &sprite)
		{
			mSprite = &sprite;

			if (mSprite->getTexture() != nullptr)
			{
				mSelectedTextureFile = mSprite->getTexture()->getFileName();
			}
			else
			{
				mSelectedTextureFile.clear();
			}
		}
	}

	void SpriteEditor::setSelectedFrame(uint32_t selectedFrame)
	{
		if (selectedFrame < (uint32_t)mSprite->mFrames.size())
		{
			mSelectedFrameIndex = selectedFrame;
			mSelectedFrame = &mSprite->mFrames[selectedFrame];
		}
	}

	void SpriteEditor::setSelectedAnimation(uint32_t selectedAnimation)
	{
		if (selectedAnimation < mSprite->mAnimations.size())
		{
			auto* animation = &mSprite->mAnimations[selectedAnimation];
			if (mSelectedAnimation != animation)
			{
				mSelectedAnimation = &mSprite->mAnimations[selectedAnimation];
			}
		}
	}

	void SpriteEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{
		if (layout.beginLayout("Sprite"))
		{
			layout.inputVec2("Size", mSprite->mSize);
			
			if (layout.fileCombo("Texture", FileType::Texture, mSelectedTextureFile))
			{
				if (!mSprite->mTexture || mSprite->mTexture->getFileName() != mSelectedTextureFile)
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

					mSprite->mTexture = cache.getResource<Texture>(mSelectedTextureFile);
				}
			}

			layout.endLayout();
		}

		if (mSelectedFrame != nullptr)
		{
			if (layout.beginLayout("Frame"))
			{
				layout.inputString("Name", mSelectedFrame->name);
				layout.inputVec2("Position", mSelectedFrame->position);
				layout.inputVec2("Size", mSelectedFrame->size);
				layout.endLayout();
			}
		}

		if (mSelectedAnimation != nullptr)
		{
			if (layout.beginLayout("Animation"))
			{
				layout.inputString("Name", mSelectedAnimation->name);

				if (layout.beginListBox("Frames"))
				{
					auto& frames = mSprite->mFrames;
					for (uint32_t idx = 0; idx < (uint32_t)mSelectedAnimation->frames.size(); idx++)
					{
						auto frameIdx = mSelectedAnimation->frames[idx];
						auto& frame = frames[frameIdx];

						if (layout.listItem(frame.name, idx == mSelectedAnimationFrame))
						{
							mSelectedAnimationFrame = idx;
						}
					}

					layout.endListBox();

					auto operation = layout.listBoxEditor();
					switch (operation)
					{
					case ListBoxOperation::Add:
						mSelectedAnimation->addFrame((uint16_t)mSelectedFrameIndex);
						break;

					case ListBoxOperation::Delete:
						if (mSelectedAnimation->frames.size() > 1)
						{
							mSelectedAnimation->removeFrame(mSelectedAnimationFrame);
						}
						break;

					case ListBoxOperation::Up:
						if (mSelectedAnimationFrame > 0)
						{
							mSelectedAnimation->moveFrame(mSelectedAnimationFrame, mSelectedAnimationFrame - 1);
							mSelectedAnimationFrame--;
						}
						break;

					case ListBoxOperation::Down:
						if (mSelectedAnimationFrame < (uint32_t)mSelectedAnimation->frames.size() - 1)
						{
							mSelectedAnimation->moveFrame(mSelectedAnimationFrame, mSelectedAnimationFrame + 1);
							mSelectedAnimationFrame++;
						}
						break;

					default:
						break;
					}
				}

				layout.endLayout();
			}
		}
	}

	void SpriteSerializer::setSprite(Sprite& sprite)
	{
		mSprite = &sprite;
	}

	bool SpriteSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		if (!reader.read(glm::value_ptr(mSprite->mSize)))
		{
			LogError("FileReader::read() failed for 'size'");
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

			mSprite->mTexture = cache.getResource<Texture>(textureFileName);
		}

		uint32_t numFrames{ 0 };
		if (!reader.read(&numFrames))
		{
			LogError("FileReader::read() failed for 'numFrames'");
			return false;
		}

		if (numFrames > 0)
		{
			for (uint32_t idx = 0; idx < numFrames; idx++)
			{
				SpriteFrame frame;	
				frame.name = reader.readString();

				if (!reader.read(glm::value_ptr(frame.position)))
				{
					LogError("FileReader::read() failed for 'frame position'");
					return false;
				}

				if (!reader.read(glm::value_ptr(frame.size)))
				{
					LogError("FileReader::read() failed for 'frame size'");
					return false;
				}

				mSprite->mFrames.push_back(std::move(frame));
			}
		}

		uint32_t numAnimations{ 0 };
		if (!reader.read(&numAnimations))
		{
			LogError("FileReader::read() failed for 'num animations'");
			return false;
		}

		if (numAnimations > 0)
		{
			for (uint32_t idx = 0; idx < numAnimations; idx++)
			{
				SpriteAnimation animation;
				animation.name = reader.readString();

				if (!reader.readVector(animation.frames))
				{
					LogError("FileReader::readVector() failed for 'animation frames'");
					return false;
				}

				mSprite->mAnimations.push_back(std::move(animation));
			}
		}

		return true;
	}

	bool SpriteSerializer::write(FileWriter& writer)
	{
		if (!writer.write(glm::value_ptr(mSprite->mSize)))
		{
			LogError("FileWriter::write() failed for 'size'");
			return false;
		}

		std::string textureFileName;
		if (mSprite->mTexture != nullptr)
		{
			textureFileName = mSprite->mTexture->getFileName();
		}

		if (!writer.writeString(textureFileName))
		{
			LogError("FileWriter::writeString() failed for '%s'", textureFileName.c_str());
			return false;
		}

		const auto numFrames = (uint32_t)mSprite->mFrames.size();
		if (!writer.write(&numFrames))
		{
			LogError("FileWriter::write() failed for 'num frames'");
			return false;
		}

		if (numFrames > 0)
		{
			for (auto& frame : mSprite->mFrames)
			{
				if (!writer.writeString(frame.name))
				{
					LogError("FileWriter::write() failed for 'frame name'");
					return false;
				}

				if (!writer.write(glm::value_ptr(frame.position)))
				{
					LogError("FileWriter::write() failed for 'frame position'");
					return false;
				}

				if (!writer.write(glm::value_ptr(frame.size)))
				{
					LogError("FileWriter::write() failed for 'frame size'");
					return false;
				}
			}
		}

		const auto numAnimations = (uint32_t)mSprite->mAnimations.size();
		if (!writer.write(&numAnimations))
		{
			LogError("FileWriter::write() failed for 'num animations'");
			return false;
		}

		if (numAnimations > 0)
		{
			for (auto& animation : mSprite->mAnimations)
			{
				if (!writer.writeString(animation.name))
				{
					LogError("FileWriter::write() failed for 'animation name'");
					return false;
				}

				if (!writer.writeVector(animation.frames))
				{
					LogError("FileWriter::write() failed for 'animation frames'");
					return false;
				}
			}
		}

		return true;
	}

	bool SpriteSerializer::read(json& object, ResourceCache& cache)
	{
		if (!object.contains("size"))
		{
			LogError("JSON sprite object doesn't have 'size' key");
			return false;
		}

		if (!object.contains("texture"))
		{
			LogError("JSON sprite object doesn't have 'texture' key");
			return false;
		}

		if (!object.contains("frames"))
		{
			LogError("JSON sprite object doesn't have 'frames' key");
			return false;
		}

		if (!object.contains("animations"))
		{
			LogError("JSON sprite object doesn't have 'animations' key");
			return false;
		}

		mSprite->mSize = {
			object["size"][0].get<float>(),
			object["size"][1].get<float>()
		};

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

			mSprite->mTexture = cache.getResource<Texture>(textureFileName);
		}

		for (auto& frameObj : object["frames"])
		{
			SpriteFrame frame;
			frame.name = frameObj["name"].get<std::string>();

			frame.position = {
				frameObj["position"][0].get<float>(),
				frameObj["position"][1].get<float>()
			};

			frame.size = {
				frameObj["size"][0].get<float>(),
				frameObj["size"][1].get<float>()
			};

			mSprite->mFrames.push_back(std::move(frame));
		}

		for (auto& animObj : object["animations"])
		{
			SpriteAnimation animation;
			animation.name = animObj["name"].get<std::string>();
			animation.frames = animObj["frames"].get<std::vector<uint16_t>>();

			mSprite->mAnimations.push_back(std::move(animation));
		}

		return true;
	}

	bool SpriteSerializer::write(json& object)
	{
		std::string textureFileName;
		if (mSprite->mTexture != nullptr)
		{
			textureFileName = mSprite->mTexture->getFileName();
		}

		json framesObj;
		for (auto& frame : mSprite->mFrames)
		{
			json frameObj;
			frameObj["name"] = frame.name;
			
			frameObj["position"] = std::vector<float>{
				frame.position.x,
				frame.position.y
			};

			frameObj["size"] = std::vector<float>{
				frame.size.x,
				frame.size.y
			};

			framesObj.push_back(std::move(frameObj));
		}

		json animationsObj;
		for (auto& animation : mSprite->mAnimations)
		{
			json animationObj;
			animationObj["name"] = animation.name;
			animationObj["frames"] = animation.frames;

			animationsObj.push_back(std::move(animationObj));
		}

		object["size"] = std::vector<float>{
			mSprite->mSize.x,
			mSprite->mSize.y
		};

		object["texture"] = textureFileName;
		object["frames"] = framesObj;
		object["animations"] = animationsObj;

		return true;
	}
}