#pragma once

#include "Core/Resource.h"
#include "VFS/Serializer.h"
#include "Editor/Editor.h"
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace Trinity
{
	class Texture;
	class SpriteEditor;
	class SpriteSerializer;
	class BatchRenderer;

	struct SpriteFrame
	{
		std::string name;
		glm::vec2 position{ 0.0f };
		glm::vec2 size{ 0.0f };
	};

	struct SpriteAnimation
	{
	public:

		static constexpr uint32_t kMaxFrames = 24;

		SpriteAnimation()
		{
			frames.reserve(kMaxFrames);
		}

		SpriteAnimation(std::string&& inName, std::vector<uint16_t>&& inFrames)
		{
			name = std::move(inName);
			frames = std::move(frames);
		}

		void addFrame(uint16_t frame)
		{
			frames.push_back(frame);
		}

		void removeFrame(uint16_t index)
		{
			frames.erase(frames.begin() + index);
		}

		void moveFrame(uint32_t from, uint32_t to)
		{
			std::swap(frames[from], frames[to]);
		}

	public:

		std::string name;
		std::vector<uint16_t> frames;
	};

	class Sprite : public Resource
	{
	public:

		static constexpr uint32_t kMaxFrames = 256;
		static constexpr uint32_t kMaxAnimations = 16;

		friend class SpriteEditor;
		friend class SpriteSerializer;

		Sprite();
		virtual ~Sprite() = default;

		Sprite(const Sprite&) = delete;
		Sprite& operator = (const Sprite&) = delete;

		Sprite(Sprite&&) = default;
		Sprite& operator = (Sprite&&) = default;

		const glm::vec2& getSize() const
		{
			return mSize;
		}

		Texture* getTexture() const
		{
			return mTexture;
		}

		const std::vector<SpriteFrame>& getFrames() const
		{
			return mFrames;
		}

		const std::vector<SpriteAnimation>& getAnimations() const
		{
			return mAnimations;
		}

		virtual std::type_index getType() const override;

		virtual IEditor* getEditor(uint32_t selectedFrame, uint32_t selectedAnimation);
		virtual ISerializer* getSerializer();

		virtual SpriteFrame* getFrame(uint32_t idx);
		virtual SpriteAnimation* getAnimation(uint32_t idx);
		virtual SpriteAnimation* getAnimation(const std::string& name);

		virtual void setSize(const glm::vec2& size);
		virtual void setTexture(Texture& texture);
		virtual void setFrames(std::vector<SpriteFrame>&& frames);
		virtual void setAnimations(std::vector<SpriteAnimation>&& animations);

		virtual void addFrame(SpriteFrame&& frame);
		virtual void removeFrame(uint32_t frameIndex);
		virtual void moveFrame(uint32_t from, uint32_t to);
		virtual void addAnimation(SpriteAnimation&& animation);
		virtual void removeAnimation(uint32_t animationIndex);

		virtual void draw(
			BatchRenderer& batchRenderer, 
			uint32_t frameIndex,
			const glm::vec2& origin,
			const glm::mat4& transform, 
			const glm::vec4& color = glm::vec4{ 0.0f },
			bool flipX = false, 
			bool flipY = false
		);

	protected:

		glm::vec2 mSize{ 0.0f };
		Texture* mTexture{ nullptr };
		std::vector<SpriteFrame> mFrames;
		std::vector<SpriteAnimation> mAnimations;
	};

	class SpriteEditor : public IEditor
	{
	public:

		SpriteEditor() = default;
		virtual ~SpriteEditor() = default;

		SpriteEditor(const SpriteEditor&) = delete;
		SpriteEditor& operator = (const SpriteEditor&) = delete;

		SpriteEditor(SpriteEditor&&) = default;
		SpriteEditor& operator = (SpriteEditor&&) = default;

		virtual void setSprite(Sprite& sprite);
		virtual void setSelectedFrame(uint32_t selectedFrame);
		virtual void setSelectedAnimation(uint32_t selectedAnimation);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		virtual void updateAnimationFrameNames();

	protected:

		Sprite* mSprite{ nullptr };
		SpriteFrame* mSelectedFrame{ nullptr };
		SpriteAnimation* mSelectedAnimation{ nullptr };
		uint32_t mSelectedFrameIndex{ 0 };
		uint32_t mSelectedAnimationFrame{ 0 };
		std::string mSelectedTextureFile;
		std::vector<const char*> mSelectedAnimationFrameNames;
	};

	class SpriteSerializer : public ISerializer
	{
	public:

		SpriteSerializer() = default;
		virtual ~SpriteSerializer() = default;

		SpriteSerializer(const SpriteSerializer&) = delete;
		SpriteSerializer& operator = (const SpriteSerializer&) = delete;

		SpriteSerializer(SpriteSerializer&&) = default;
		SpriteSerializer& operator = (SpriteSerializer&&) = default;

		virtual void setSprite(Sprite& sprite);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		Sprite* mSprite{ nullptr };
	};
}