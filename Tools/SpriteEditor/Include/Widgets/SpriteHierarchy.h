#pragma once

#include "Core/EditorWidget.h"
#include "Core/Observer.h"
#include <string>
#include <memory>

namespace Trinity
{
	class Sprite;
	struct SpriteFrame;
	struct SpriteAnimation;

	class SpriteHierarchy : public EditorWidget
	{
	public:

		SpriteHierarchy() = default;
		virtual ~SpriteHierarchy() = default;

		SpriteHierarchy(const SpriteHierarchy&) = delete;
		SpriteHierarchy& operator = (const SpriteHierarchy&) = delete;

		SpriteHierarchy(SpriteHierarchy&&) = default;
		SpriteHierarchy& operator = (SpriteHierarchy&&) = default;

		uint32_t getSelectedFrame() const
		{
			return mSelectedFrame;
		}

		uint32_t getSelectedAnimation() const
		{
			return mSelectedAnimation;
		}

		virtual void setSprite(Sprite& sprite);
		virtual void setSelectedFrame(uint32_t selectedFrame);
		virtual void draw() override;

	public:

		Observer<uint32_t> onSelectFrame;
		Observer<uint32_t> onSelectAnimation;

	protected:

		Sprite* mSprite{ nullptr };
		uint32_t mSelectedFrame{ 0 };
		uint32_t mSelectedAnimation{ 0 };
	};
}