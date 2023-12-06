#include "Widgets/SpriteHierarchy.h"
#include "Scene/Sprite.h"
#include "Utils/EditorHelper.h"
#include <format>
#include "IconsFontAwesome6.h"

namespace Trinity
{
	void SpriteHierarchy::setSprite(Sprite& sprite)
	{
		mSprite = &sprite;
		updateNames();
	}

	void SpriteHierarchy::draw()
	{
		if (!isEnabled())
		{
			return;
		}

		ImGui::Begin(mTitle.c_str(), &mEnabled);
		{
			ImGui::SetNextItemWidth(-FLT_MIN);

			if (ImGui::CollapsingHeader("Frames", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SetNextItemWidth(-FLT_MIN);

				auto operation = EditorHelper::listBox("##frames", (int32_t&)mSelectedFrame, mFrameNames);
				switch (operation)
				{
				case ListBoxOperation::Select:
					onSelectFrame.notify(mSelectedFrame);
					break;

				case ListBoxOperation::Add:
				{
					SpriteFrame frame = {
						.name = std::format("frame{}", (uint32_t)mFrameNames.size()),
						.position = glm::vec2{ 0.0f },
						.size = mSprite->getSize()
					};

					mSprite->addFrame(std::move(frame));
					updateNames();

					break;
				}

				case ListBoxOperation::Delete:
				{
					if (mFrameNames.size() > 1)
					{
						mSprite->removeFrame(mSelectedFrame);
						updateNames();
					}
					break;
				}

				case ListBoxOperation::Up:
				{
					if (mSelectedFrame > 0)
					{
						mSprite->moveFrame(mSelectedFrame, mSelectedFrame - 1);
						updateNames();

						mSelectedFrame--;
					}
					break;
				}

				case ListBoxOperation::Down:
				{
					if (mSelectedFrame < (uint32_t)mFrameNames.size() - 1)
					{
						mSprite->moveFrame(mSelectedFrame, mSelectedFrame + 1);
						updateNames();

						mSelectedFrame++;
					}
					break;
				}

				default:
					break;
				}
			}

			ImGui::SetNextItemWidth(-FLT_MIN);

			if (ImGui::CollapsingHeader("Animations", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				
				auto operation = EditorHelper::listBox("##animations", (int32_t&)mSelectedAnimation, mAnimationNames);
				switch (operation)
				{
				case ListBoxOperation::Select:
					break;
				case ListBoxOperation::Add:
					break;
				case ListBoxOperation::Delete:
					break;
				case ListBoxOperation::Up:
					break;
				case ListBoxOperation::Down:
					break;
				default:
					break;
				}
			}

			ImGui::End();
		}
	}

	void SpriteHierarchy::updateNames()
	{
		if (mSprite != nullptr)
		{
			mFrameNames.clear();
			mAnimationNames.clear();

			for (const auto& frame : mSprite->getFrames())
			{
				mFrameNames.push_back(frame.name.c_str());
			}

			for (const auto& animation : mSprite->getAnimations())
			{
				mAnimationNames.push_back(animation.name.c_str());
			}
		}
	}
}