#include "Widgets/ParticleHierarchy.h"
#include "Particle/ParticleEffect.h"
#include "Particle/ParticleEmitter.h"
#include "Utils/EditorHelper.h"
#include <format>
#include "IconsFontAwesome6.h"

namespace Trinity
{
	void ParticleHierarchy::setParticle(ParticleEffect& particle)
	{
		mParticle = &particle;
	}

	void ParticleHierarchy::setSelectedEmitter(uint32_t selectedEmitter)
	{
		mSelectedEmitter = selectedEmitter;
	}

	void ParticleHierarchy::draw()
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

			auto emitters = mParticle->getEmitters();
			if (ImGui::CollapsingHeader("Emitters", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SetNextItemWidth(-FLT_MIN);

				if (ImGui::BeginTable("##emitters", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2{ 0.0f, tableHeight }))
				{
					for (uint32_t idx = 0; idx < (uint32_t)emitters.size(); idx++)
					{
						ImGui::SetNextItemWidth(-FLT_MIN);
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::AlignTextToFramePadding();

						bool selected = mSelectedEmitter == idx;
						if (ImGui::Selectable(emitters[idx]->getName().c_str(), &selected))
						{
							if (idx != mSelectedEmitter)
							{
								mSelectedEmitter = idx;
								onSelectEmitter.notify(mSelectedEmitter);
							}
						}

						if (selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndTable();
				}

				auto operation = EditorHelper::listBoxEditor("##emitter-editor");
				switch (operation)
				{
				case ListBoxOperation::Add:
				{
					auto emitter = std::make_unique<ParticleEmitter>();
					emitter->setName(std::format("emitter{}", emitters.size()));
					mParticle->addEmitter(std::move(emitter));
					break;
				}

				case ListBoxOperation::Delete:
				{
					if (emitters.size() > 1)
					{
						mParticle->removeEmitter(mSelectedEmitter);
					}
					break;
				}

				case ListBoxOperation::Up:
				{
					if (mSelectedEmitter > 0)
					{
						mParticle->moveEmitter(mSelectedEmitter, mSelectedEmitter - 1);
						mSelectedEmitter--;
					}
					break;
				}

				case ListBoxOperation::Down:
				{
					if (mSelectedEmitter < (uint32_t)emitters.size() - 1)
					{
						mParticle->moveEmitter(mSelectedEmitter, mSelectedEmitter + 1);
						mSelectedEmitter++;
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