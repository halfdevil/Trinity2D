#include "Widgets/SpriteViewport.h"
#include "Scene/Sprite.h"
#include "Math/BoundingRect.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderPass.h"
#include "Graphics/LineCanvas.h"
#include "Core/EditorTheme.h"
#include "Core/EditorResources.h"
#include "Core/EditorGizmo.h"
#include "Core/EditorCamera.h"
#include "Core/EditorGrid.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	bool SpriteViewport::create(EditorResources& resources)
	{
		if (!Viewport::create(resources))
		{
			LogError("Viewport::create() failed");
			return false;
		}

		mRenderer = std::make_unique<BatchRenderer>();
		if (!mRenderer->create(*mFrameBuffer, *resources.getResourceCache(), 
			kTexturedShader, kColoredShader))
		{
			LogError("BatchRenderer::create() failed with shader: '%s' and '%s'", 
				kTexturedShader, kColoredShader);
			return true;
		}

		return true;
	}

	void SpriteViewport::destroy()
	{
		Viewport::destroy();
		mRenderer = nullptr;
	}

	void SpriteViewport::setSprite(Sprite& sprite)
	{
		mSprite = &sprite;
	}

	void SpriteViewport::setSelectedFrame(uint32_t selectedFrame)
	{
		mSelectedFrame = selectedFrame;
	}

	void SpriteViewport::drawContent(float deltaTime)
	{		
		mRenderPass->begin(*mFrameBuffer);

		if (mGrid != nullptr)
		{
			mGrid->draw(*mRenderPass);
		}

		if (mCamera != nullptr)
		{
			mRenderer->begin(mCamera->getViewProj());
			drawSprite(mSprite);
			mRenderer->end(*mRenderPass);

			if (mLineCanvas != nullptr)
			{
				mLineCanvas->draw(mCamera->getViewProj(), *mRenderPass);
			}
		}

		mRenderPass->end();
	}

	void SpriteViewport::drawSprite(Sprite* sprite)
	{
		if (sprite->getTexture() != nullptr)
		{
			float width = mCamera->getSize().x;
			float height = mCamera->getSize().y;
			float itemWidth = sprite->getSize().x + 2 * mPadding.x;
			float itemHeight = sprite->getSize().y + 2 * mPadding.y;

			auto& frames = sprite->getFrames();
			auto* texture = sprite->getTexture();
			auto numFrames = (uint32_t)frames.size();

			auto numColumns = (uint32_t)(width / itemWidth);
			if (numColumns == 0)
			{
				numColumns++;
			}

			auto numRows = numFrames / numColumns;
			if ((numFrames % numColumns) != 0)
			{
				numRows++;
			}

			float totalWidth = numColumns * itemWidth;
			float totalHeight = numRows * itemHeight;

			if (numFrames < numColumns)
			{
				totalWidth = numFrames * itemWidth;
			}

			glm::vec2 position{ 
				-(totalWidth / 2.0f), 
				(totalHeight / 2.0f) - itemHeight
			};

			for (uint32_t idx = 0; idx < numFrames; idx++)
			{
				auto& frame = frames[idx];
				mRenderer->drawTexture(
					texture,
					frame.position,
					frame.size,
					position,
					frame.size,
					glm::vec2{ 0.0f },
					glm::mat4(1.0f)
				);

				if (idx == mSelectedFrame)
				{
					mLineCanvas->rect(position, frame.size,	glm::vec4{ 1.0f, 0.6f, 0.0f, 1.0f });
				}

				if (((idx + 1) % numColumns) == 0)
				{
					position = glm::vec2{ -(totalWidth / 2.0f), position.y - itemHeight };
				}
				else
				{
					position.x += itemWidth;
				}
			}
		}
	}

	void SpriteViewport::onViewportResize(uint32_t width, uint32_t height)
	{
		Viewport::onViewportResize(width, height);

		if (mCamera != nullptr)
		{
			float halfWidth = 0.5f * width;
			float halfHeight = 0.5f * height;

			mCamera->setSize(-halfWidth, halfWidth, -halfHeight, halfHeight);
		}

		if (mGrid != nullptr)
		{
			mGrid->setCanvasSize(glm::vec2{ (float)width, (float)height });
			mGrid->updateGridData();
		}
	}
}