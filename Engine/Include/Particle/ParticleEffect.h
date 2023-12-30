#pragma once

#include "Core/Resource.h"
#include "Editor/Editor.h"
#include "VFS/Serializer.h"
#include "glm/glm.hpp"
#include <memory>
#include <vector>

namespace Trinity
{
	class Particle;
	class ParticleEmitter;
	class ParticlePool;
	class BatchRenderer;
	class ParticleEditor;
	class ParticleSerializer;

	class ParticleEffect : public Resource
	{
	public:

		static constexpr uint32_t kParticlePoolSize = 2048;

		friend class ParticleEditor;
		friend class ParticleSerializer;

		ParticleEffect() = default;
		virtual ~ParticleEffect() = default;

		ParticleEffect(const ParticleEffect&) = delete;
		ParticleEffect& operator = (const ParticleEffect&) = delete;

		ParticleEffect(ParticleEffect&&) = default;
		ParticleEffect& operator = (ParticleEffect&&) = default;

		const glm::mat4& getTransform() const
		{
			return mTransform;
		}

		virtual std::type_index getType() const;

		virtual std::vector<ParticleEmitter*> getEmitters() const;
		virtual IEditor* getEditor(uint32_t selectedEmitter);
		virtual ISerializer* getSerializer();

		virtual ParticleEmitter* getEmitter(uint32_t index) const;
		virtual void addEmitter(std::unique_ptr<ParticleEmitter> emitter);
		virtual void removeEmitter(uint32_t index);
		virtual void moveEmitter(uint32_t from, uint32_t to);

		virtual void init(const glm::vec2& position, float rotation, 
			uint32_t poolSize = kParticlePoolSize);

		virtual void setTransfrom(const glm::mat4& transform);
		virtual void update(float deltaTime);
		virtual void draw(BatchRenderer& batchRenderer);

	protected:

		std::vector<std::unique_ptr<ParticleEmitter>> mEmitters;
		std::unique_ptr<ParticlePool> mPool{ nullptr };
		glm::mat4 mTransform{ 1.0f };
	};

	class ParticleEditor : public IEditor
	{
	public:

		ParticleEditor() = default;
		virtual ~ParticleEditor() = default;

		ParticleEditor(const ParticleEditor&) = delete;
		ParticleEditor& operator = (const ParticleEditor&) = delete;

		ParticleEditor(ParticleEditor&&) = default;
		ParticleEditor& operator = (ParticleEditor&&) = default;

		virtual void setParticleEffect(ParticleEffect& effect);
		virtual void setSelectedEmitter(uint32_t selectedEmitter);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		ParticleEffect* mParticle{ nullptr };
		ParticleEmitter* mSelectedEmitter{ nullptr };
	};

	class ParticleSerializer : public ISerializer
	{
	public:

		ParticleSerializer() = default;
		virtual ~ParticleSerializer() = default;

		ParticleSerializer(const ParticleSerializer&) = delete;
		ParticleSerializer& operator = (const ParticleSerializer&) = delete;

		ParticleSerializer(ParticleSerializer&&) = default;
		ParticleSerializer& operator = (ParticleSerializer&&) = default;

		virtual void setParticleEffect(ParticleEffect& effect);

		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;
		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		ParticleEffect* mParticle{ nullptr };
	};
}