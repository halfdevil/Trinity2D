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
	}

	void SpriteHierarchy::setSelectedFrame(uint32_t selectedFrame)
	{
		mSelectedFrame = selectedFrame;
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

			const float tableHeight = ImGui::GetTextLineHeightWithSpacing() * 7.25f +
				ImGui::GetStyle().FramePadding.y * 2.0f;

			const auto& frames = mSprite->getFrames();
			const auto& animations = mSprite->getAnimations();

			if (ImGui::CollapsingHeader("Frames", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SetNextItemWidth(-FLT_MIN);

				if (ImGui::BeginTable("##frames", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2{ 0.0f, tableHeight }))
				{
					for (uint32_t idx = 0; idx < (uint32_t)frames.size(); idx++)
					{
						ImGui::SetNextItemWidth(-FLT_MIN);
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::AlignTextToFramePadding();

						bool selected = mSelectedFrame == idx;
						if (ImGui::Selectable(frames[idx].name.c_str(), &selected))
						{
							if (idx != mSelectedFrame)
							{
								mSelectedFrame = idx;
								onSelectFrame.notify(mSelectedFrame);
							}
						}

						if (selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndTable();
				}

				auto operation = EditorHelper::listBoxEditor("##frames-editor");
				switch (operation)
				{
				case ListBoxOperation::Add:
				{
					SpriteFrame frame = {
						.name = std::format("frame{}", (uint32_t)frames.size()),
						.position = glm::vec2{ 0.0f },
						.size = mSprite->getSize()
					};

					mSprite->addFrame(std::move(frame));
					break;
				}

				case ListBoxOperation::Delete:
				{
					if (frames.size() > 1)
					{
						mSprite->removeFrame(mSelectedFrame);
					}
					break;
				}

				case ListBoxOperation::Up:
				{
					if (mSelectedFrame > 0)
					{
						mSprite->moveFrame(mSelectedFrame, mSelectedFrame - 1);
						mSelectedFrame--;
					}
					break;
				}

				case ListBoxOperation::Down:
				{
					if (mSelectedFrame < (uint32_t)frames.size() - 1)
					{
						mSprite->moveFrame(mSelectedFrame, mSelectedFrame + 1);
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

				if (ImGui::BeginTable("##animations", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2{ 0.0f, tableHeight }))
				{					
					for (uint32_t idx = 0; idx < (uint32_t)animations.size(); idx++)
					{
						ImGui::SetNextItemWidth(-FLT_MIN);
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::AlignTextToFramePadding();

						bool selected = mSelectedAnimation == idx;
						if (ImGui::Selectable(animations[idx].name.c_str(), &selected))
						{
							if (idx != mSelectedAnimation)
							{
								mSelectedAnimation = idx;
								onSelectAnimation.notify(mSelectedAnimation);
							}
						}

						if (selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndTable();
				}

				auto operation = EditorHelper::listBoxEditor("##animations-editor");
				switch (operation)
				{
				case ListBoxOperation::Add:
				{
					SpriteAnimation animation = {
						std::format("animation{}", (uint32_t)animations.size()),
						{ 0 }
					};

					mSprite->addAnimation(std::move(animation));
					break;
				}

				case ListBoxOperation::Delete:
				{
					if (animations.size() > 1)
					{
						mSprite->removeFrame(mSelectedFrame);
					}
					break;
				}

				case ListBoxOperation::Up:
				{
					if (mSelectedFrame > 0)
					{
						mSprite->moveFrame(mSelectedFrame, mSelectedFrame - 1);
						mSelectedFrame--;
					}
					break;
				}

				case ListBoxOperation::Down:
				{
					if (mSelectedFrame < (uint32_t)animations.size() - 1)
					{
						mSprite->moveFrame(mSelectedFrame, mSelectedFrame + 1);
						mSelectedFrame++;
					}
					break;
				}

				default:
					break;
				}
			}

			ImGui::End();
		}
	}
}