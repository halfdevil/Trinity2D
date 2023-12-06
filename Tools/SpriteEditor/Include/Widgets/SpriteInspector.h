#pragma once

#include "Widgets/Inspector.h"

namespace Trinity
{
	class Sprite;
	struct SpriteFrame;
	struct SpriteAnimation;

	class SpriteInspector : public Inspector
	{
	public:

		SpriteInspector() = default;
		virtual ~SpriteInspector() = default;

		SpriteInspector(const SpriteInspector&) = delete;
		SpriteInspector& operator = (const SpriteInspector&) = delete;

		SpriteInspector(SpriteInspector&&) = default;
		SpriteInspector& operator = (SpriteInspector&&) = default;

		virtual void setSprite(Sprite& sprite);
		virtual void setSelectedFrame(uint32_t selectedFrame);
		virtual void setSelectedAnimation(uint32_t selectedAnimation);

	protected:

		virtual void drawInspector() override;

	protected:

		Sprite* mSprite{ nullptr };
		uint32_t mSelectedFrame{ 0 };
		uint32_t mSelectedAnimation{ 0 };
		std::string mSpriteName;
	};
}