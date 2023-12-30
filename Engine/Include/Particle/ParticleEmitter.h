#pragma once

#include "Core/ObjectPool.h"
#include "Editor/Editor.h"
#include "VFS/Serializer.h"
#include "Utils/RandomGenerator.h"
#include "glm/glm.hpp"

namespace Trinity
{
	class Texture;
	class Particle;
	class ParticlePool;
	class ParticleEmitterEditor;
	class ParticleEmitterSerializer;

	enum class DistributionType
	{
		Constant,
		Random
	};

	struct FloatParticleParameter
	{
		float minValue{ 0.0f };
		float maxValue{ 0.0f };
		DistributionType distributionType{ DistributionType::Constant };
	};

	struct Vec2ParticleParameter
	{
		glm::vec2 minValue{ 0.0f };
		glm::vec2 maxValue{ 0.0f };
		DistributionType distributionType{ DistributionType::Constant };
	};

	struct Vec4ParticleParameter
	{
		glm::vec4 minValue{ 0.0f };
		glm::vec4 maxValue{ 0.0f };
		DistributionType distributionType{ DistributionType::Constant };
	};

	class ParticleEmitter
	{
	public:

		friend class ParticleEmitterEditor;
		friend class ParticleEmitterSerializer;

		ParticleEmitter() = default;
		virtual ~ParticleEmitter() = default;

		ParticleEmitter(const ParticleEmitter&) = delete;
		ParticleEmitter& operator = (const ParticleEmitter&) = delete;

		ParticleEmitter(ParticleEmitter&&) = default;
		ParticleEmitter& operator = (ParticleEmitter&&) = default;

		const std::string& getName() const
		{
			return mName;
		}

		uint32_t getTotalPartices() const
		{
			return mTotalParticles;
		}

		bool isExpired() const
		{
			return mTotalParticles != 0 && mNumRemaining == 0;
		}

		virtual IEditor* getEditor();
		virtual ISerializer* getSerializer();

		virtual void emitParticles(ParticlePool& particlePool);
		virtual void setName(const std::string& name);
		virtual void setTotalParticles(uint32_t totalParticles);
		virtual void setTexture(Texture& texture);
		virtual void setSrcPosition(const glm::vec2& srcPosition);
		virtual void setSrcSize(const glm::vec2& srcSize);
		virtual void setNumToEmit(const FloatParticleParameter& numToEmit);
		virtual void setPosition(const Vec2ParticleParameter& position);
		virtual void setVelocity(const Vec2ParticleParameter& velocity);
		virtual void setAcceleration(const Vec2ParticleParameter& acceleration);
		virtual void setStartColor(const Vec4ParticleParameter& startColor);
		virtual void setFinalColor(const Vec4ParticleParameter& finalColor);
		virtual void setStartSize(const FloatParticleParameter& startSize);
		virtual void setFinalSize(const FloatParticleParameter& finalSize);
		virtual void setStartRotation(const FloatParticleParameter& startRotation);
		virtual void setFinalRotation(const FloatParticleParameter& finalRotation);
		virtual void setDrag(const FloatParticleParameter& drag);
		virtual void setLifeSpan(const FloatParticleParameter& lifeSpan);

	protected:

		virtual void initParticleSprite(Particle& particleSprite);
		virtual float getParameterValue(const FloatParticleParameter& parameter);
		virtual glm::vec2 getParameterValue(const Vec2ParticleParameter& parameter);
		virtual glm::vec4 getParameterValue(const Vec4ParticleParameter& parameter);

	protected:

		std::string mName;
		uint32_t mTotalParticles;
		uint32_t mNumRemaining{ 0 };
		RandomGenerator<std::mt19937> mRandom;
		Texture* mTexture{ nullptr };
		glm::vec2 mSrcPosition{ 0.0f };
		glm::vec2 mSrcSize{ 0.0f };
		FloatParticleParameter mNumToEmit;
		Vec2ParticleParameter mPosition;
		Vec2ParticleParameter mVelocity;
		Vec2ParticleParameter mAcceleration;
		Vec4ParticleParameter mStartColor;
		Vec4ParticleParameter mFinalColor;
		FloatParticleParameter mStartSize;
		FloatParticleParameter mFinalSize;
		FloatParticleParameter mStartRotation;
		FloatParticleParameter mFinalRotation;
		FloatParticleParameter mDrag;
		FloatParticleParameter mLifeSpan;
	};

	class ParticleEmitterEditor : public IEditor
	{
	public:

		ParticleEmitterEditor() = default;
		virtual ~ParticleEmitterEditor() = default;

		ParticleEmitterEditor(const ParticleEmitterEditor&) = delete;
		ParticleEmitterEditor& operator = (const ParticleEmitterEditor&) = delete;

		ParticleEmitterEditor(ParticleEmitterEditor&&) = default;
		ParticleEmitterEditor& operator = (ParticleEmitterEditor&&) = default;

		virtual void setParticleEmitter(ParticleEmitter& emitter);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		virtual const char* getDistributionTypeStr(DistributionType distributionType) const;
		virtual void distributionTypeGui(const IEditorLayout& layout, DistributionType& distributionType);
		virtual void parameterGui(const IEditorLayout& layout, const std::string& name, FloatParticleParameter& parameter);
		virtual void parameterGui(const IEditorLayout& layout, const std::string& name, Vec2ParticleParameter& parameter);
		virtual void parameterGui(const IEditorLayout& layout, const std::string& name, Vec4ParticleParameter& parameter);

	protected:

		ParticleEmitter* mParticleEmitter{ nullptr };
		std::string mSelectedTextureFile;
	};

	class ParticleEmitterSerializer : public ISerializer
	{
	public:

		ParticleEmitterSerializer() = default;
		virtual ~ParticleEmitterSerializer() = default;

		ParticleEmitterSerializer(const ParticleEmitterSerializer&) = delete;
		ParticleEmitterSerializer& operator = (const ParticleEmitterSerializer&) = delete;

		ParticleEmitterSerializer(ParticleEmitterSerializer&&) = default;
		ParticleEmitterSerializer& operator = (ParticleEmitterSerializer&&) = default;

		virtual void setParticleEmitter(ParticleEmitter& emitter);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		virtual bool readParameter(FileReader& reader, FloatParticleParameter& parameter);
		virtual bool readParameter(FileReader& reader, Vec2ParticleParameter& parameter);
		virtual bool readParameter(FileReader& reader, Vec4ParticleParameter& parameter);

		virtual bool writeParameter(FileWriter& writer, const FloatParticleParameter& parameter);
		virtual bool writeParameter(FileWriter& writer, const Vec2ParticleParameter& parameter);
		virtual bool writeParameter(FileWriter& writer, const Vec4ParticleParameter& parameter);

		virtual bool readParameter(const json& object, FloatParticleParameter& parameter);
		virtual bool readParameter(const json& object, Vec2ParticleParameter& parameter);
		virtual bool readParameter(const json& object, Vec4ParticleParameter& parameter);

		virtual bool writeParameter(json& object, const FloatParticleParameter& parameter);
		virtual bool writeParameter(json& object, const Vec2ParticleParameter& parameter);
		virtual bool writeParameter(json& object, const Vec4ParticleParameter& parameter);

	protected:

		ParticleEmitter* mParticleEmitter{ nullptr };
	};
}