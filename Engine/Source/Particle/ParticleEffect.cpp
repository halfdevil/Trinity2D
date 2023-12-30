#include "Particle/ParticleEffect.h"
#include "Particle/ParticleEmitter.h"
#include "Particle/ParticlePool.h"
#include "Particle/Particle.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	std::type_index ParticleEffect::getType() const
	{
		return typeid(ParticleEffect);
	}

	std::vector<ParticleEmitter*> ParticleEffect::getEmitters() const
	{
		std::vector<ParticleEmitter*> result(mEmitters.size());
		std::transform(mEmitters.begin(), mEmitters.end(), result.begin(), 
			[](const auto& emitter) {
				return emitter.get();
			}
		);

		return result;
	}

	IEditor* ParticleEffect::getEditor(uint32_t selectedEmitter)
	{
		static ParticleEditor editor;
		editor.setParticleEffect(*this);
		editor.setSelectedEmitter(selectedEmitter);

		return &editor;
	}

	Trinity::ISerializer* ParticleEffect::getSerializer()
	{
		static ParticleSerializer serializer;
		serializer.setParticleEffect(*this);

		return &serializer;
	}

	ParticleEmitter* ParticleEffect::getEmitter(uint32_t index) const
	{
		if (index < (uint32_t)mEmitters.size())
		{
			return mEmitters.at(index).get();
		}

		return nullptr;
	}

	void ParticleEffect::addEmitter(std::unique_ptr<ParticleEmitter> emitter)
	{
		mEmitters.push_back(std::move(emitter));
	}

	void ParticleEffect::removeEmitter(uint32_t index)
	{
		if (index < (uint32_t)mEmitters.size())
		{
			mEmitters.erase(mEmitters.begin() + index);
		}
	}

	void ParticleEffect::moveEmitter(uint32_t from, uint32_t to)
	{
		if (from < (uint32_t)mEmitters.size() && to < (uint32_t)mEmitters.size())
		{
			std::swap(mEmitters[from], mEmitters[to]);
		}
	}

	void ParticleEffect::init(const glm::vec2& position, float rotation, uint32_t poolSize)
	{
		mTransform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) *
			glm::yawPitchRoll(0.0f, 0.0f, rotation);

		mPool = std::make_unique<ParticlePool>();
		mPool->init(poolSize);
	}

	void ParticleEffect::setTransfrom(const glm::mat4& transform)
	{
		mTransform = transform;
	}

	void ParticleEffect::update(float deltaTime)
	{
		mPool->update(deltaTime);

		for (uint32_t idx = 0; idx < (uint32_t)mEmitters.size(); idx++)
		{
			auto& emitter = mEmitters.at(idx);
			if (!emitter->isExpired())
			{
				emitter->emitParticles(*mPool);
			}
		}
	}

	void ParticleEffect::draw(BatchRenderer& batchRenderer)
	{
		mPool->draw(batchRenderer, mTransform);
	}

	void ParticleEditor::setParticleEffect(ParticleEffect& effect)
	{
		mParticle = &effect;
	}

	void ParticleEditor::setSelectedEmitter(uint32_t selectedEmitter)
	{
		if (selectedEmitter < (uint32_t)mParticle->mEmitters.size())
		{
			mSelectedEmitter = mParticle->mEmitters[selectedEmitter].get();
		}
		else
		{
			mSelectedEmitter = nullptr;
		}
	}

	void ParticleEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{
		if (mSelectedEmitter != nullptr)
		{
			if (auto* editor = mSelectedEmitter->getEditor(); editor != nullptr)
			{
				editor->onInspectorGui(layout, cache);
			}
		}
	}

	void ParticleSerializer::setParticleEffect(ParticleEffect& effect)
	{
		mParticle = &effect;
	}

	bool ParticleSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		uint32_t numEmitters{ 0 };
		if (!reader.read(&numEmitters))
		{
			LogError("FileReader::read() failed for 'num emitters'");
			return false;
		}

		for (uint32_t idx = 0; idx < numEmitters; idx++)
		{
			auto emitter = std::make_unique<ParticleEmitter>();
			if (auto* serializer = emitter->getSerializer(); serializer != nullptr)
			{
				if (!serializer->read(reader, cache))
				{
					LogError("ParticleEmitterSerializer::read() failed");
					return false;
				}
			}

			mParticle->mEmitters.push_back(std::move(emitter));
		}

		return true;
	}

	bool ParticleSerializer::write(FileWriter& writer)
	{
		auto numEmitters = (uint32_t)mParticle->mEmitters.size();
		if (!writer.write(&numEmitters))
		{
			LogError("FileWriter::write() failed for 'num emitters'");
			return false;
		}

		for (auto& emitter : mParticle->mEmitters)
		{
			if (auto* serializer = emitter->getSerializer(); serializer != nullptr)
			{
				if (!serializer->write(writer))
				{
					LogError("ParticleEmitterSerializer::write() failed");
					return false;
				}
			}
		}

		return true;
	}

	bool ParticleSerializer::read(json& object, ResourceCache& cache)
	{
		if (!object.contains("emitters"))
		{
			LogError("Particle Effect JSON object doesn't contain 'emitters' key");
			return false;
		}

		for (auto& emitterJson : object["emitters"])
		{
			auto emitter = std::make_unique<ParticleEmitter>();
			if (auto* serializer = emitter->getSerializer(); serializer != nullptr)
			{
				if (!serializer->read(emitterJson, cache))
				{
					LogError("ParticleEmitterSerializer::read() failed");
					return false;
				}
			}

			mParticle->mEmitters.push_back(std::move(emitter));
		}

		return true;
	}

	bool ParticleSerializer::write(json& object)
	{
		json emittersJson;
		for (auto& emitter : mParticle->mEmitters)
		{
			json emitterJson;
			if (auto* serializer = emitter->getSerializer(); serializer != nullptr)
			{
				if (!serializer->write(emitterJson))
				{
					LogError("ParticleEmitterSerializer::write() failed");
					return false;
				}
			}

			emittersJson.push_back(std::move(emitterJson));
		}

		object["emitters"] = std::move(emittersJson);
		return true;
	}
}