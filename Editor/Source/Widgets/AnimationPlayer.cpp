#include "Widgets/AnimationPlayer.h"
#include "Scene/Scene.h"
#include "Scene/SceneSystem.h"
#include "Physics/Physics.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Core/EditorTheme.h"
#include "Core/EditorResources.h"
#include "Core/EditorCamera.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "IconsFontAwesome6.h"
#include <format>

namespace Trinity
{
	bool AnimationPlayer::create(EditorResources& resources)
	{
		if (!Viewport::create(resources, 0, glm::vec4{ 0.3f, 0.3f, 0.3f, 1.0f }))
		{
			LogError("Viewport::create() failed");
			return false;
		}

		mSceneSystem = std::make_unique<SceneSystem>();
		if (!mSceneSystem->create(*mFrameBuffer, *resources.getResourceCache()))
		{
			LogError("SceneSystem::create() failed");
			return false;
		}

		auto& theme = EditorTheme::get();
		auto scaleFactor = theme.getScaleFactor();

		mShowToolbar = false;
		mShowBottomPanel = true;
		mBottomPanelHeight = kBottomPanelHeight * scaleFactor;

		return true;
	}

	void AnimationPlayer::destroy()
	{
		Viewport::destroy();
	}

	void AnimationPlayer::setScene(Scene& scene)
	{
		mScene = &scene;
		mSceneSystem->setScene(scene);
	}

	void AnimationPlayer::update(float deltaTime)
	{
		Viewport::update(deltaTime);
	}

	void AnimationPlayer::drawContent(float deltaTime)
	{
		mRenderPass->begin(*mFrameBuffer);
		mSceneSystem->draw(*mRenderPass, mCamera->getViewProj());
		mRenderPass->end();
	}

	void AnimationPlayer::drawBottomPanel(float x, float y, float width, float height)
	{
		ImGuiWindowFlags windowFlags = 0
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoSavedSettings;

		ImGui::SetNextWindowPos({ x, y });
		ImGui::BeginChild("##bottomPanel", ImVec2{ width, height }, false, windowFlags);
		{
			ImGui::Separator();

			if (ImGui::Button(!mPlaying ? ICON_FA_PLAY : ICON_FA_PAUSE))
			{
				mPlaying = !mPlaying;
				onPlayPauseClick(mPlaying);
			}

			ImGui::SameLine();

			if (ImGui::Button(ICON_FA_STOP))
			{
				mPlaying = false;
				onStopClick();
			}
			
			ImGui::SameLine();
			ImGui::TextUnformatted("Loop:");
			ImGui::SameLine();

			if (ImGui::Checkbox("##loop", &mLooping))
			{
				onLoopClick(mLooping);
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Speed:");
			ImGui::SameLine();

			if (ImGui::BeginCombo("##speed", getSpeedStr(mSpeed).c_str(), ImGuiComboFlags_WidthFitPreview))
			{
				uint32_t numSpeeds = sizeof(kAnimationSpeeds) / sizeof(float);
				for (uint32_t idx = 0; idx < numSpeeds; idx++)
				{
					bool selected = mSpeed == kAnimationSpeeds[idx];
					if (ImGui::Selectable(getSpeedStr(kAnimationSpeeds[idx]).c_str(), selected))
					{
						mSpeed = kAnimationSpeeds[idx];
						onSpeedChange(mSpeed);
					}

					if (selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			ImGui::EndChild();
		}
	}

	void AnimationPlayer::onPlayPauseClick(bool playing)
	{
	}

	void AnimationPlayer::onStopClick()
	{
	}

	void AnimationPlayer::onSpeedChange(float speed)
	{
	}

	void AnimationPlayer::onLoopClick(bool loop)
	{
	}

	std::string AnimationPlayer::getSpeedStr(float speed)
	{
		if (speed == 1.0f)
		{
			return "Normal";
		}

		return std::format("{:.2f}", speed);
	}
}