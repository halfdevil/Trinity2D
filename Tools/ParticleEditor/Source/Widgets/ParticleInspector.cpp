#include "Widgets/ParticleInspector.h"
#include "Particle/ParticleEffect.h"
#include "Particle/ParticleEmitter.h"
#include "Editor/Editor.h"
#include "Core/EditorLayout.h"
#include "Core/EditorTheme.h"
#include "misc/cpp/imgui_stdlib.h"

namespace Trinity
{
	void ParticleInspector::setParticle(ParticleEffect& sprite)
	{
		mParticle = &sprite;
	}

	void ParticleInspector::setSelectedEmitter(uint32_t selectedEmitter)
	{
		mSelectedEmitter = selectedEmitter;
	}

	void ParticleInspector::drawInspector()
	{
		if (mParticle != nullptr)
		{
			if (auto* editor = mParticle->getEditor(mSelectedEmitter); editor != nullptr)
			{
				editor->onInspectorGui(*mLayout, *mResourceCache);
			}
		}
	}
}