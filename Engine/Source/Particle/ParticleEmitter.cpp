#include "Particle/ParticleEmitter.h"
#include "Particle/ParticlePool.h"
#include "Particle/Particle.h"
#include "Editor/EditorLayout.h"
#include "Graphics/Texture.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	IEditor* ParticleEmitter::getEditor()
	{
		static ParticleEmitterEditor editor;
		editor.setParticleEmitter(*this);

		return &editor;
	}

	ISerializer* ParticleEmitter::getSerializer()
	{
		static ParticleEmitterSerializer serializer;
		serializer.setParticleEmitter(*this);

		return &serializer;
	}

	void ParticleEmitter::setName(const std::string& name)
	{
		mName = name;
	}

	void ParticleEmitter::setTotalParticles(uint32_t totalParticles)
	{
		mTotalParticles = totalParticles;
		mNumRemaining = totalParticles;
	}

	void ParticleEmitter::setTexture(Texture& texture)
	{
		mTexture = &texture;
	}

	void ParticleEmitter::setSrcPosition(const glm::vec2& srcPosition)
	{
		mSrcPosition = srcPosition;
	}

	void ParticleEmitter::setSrcSize(const glm::vec2& srcSize)
	{
		mSrcSize = srcSize;
	}

	void ParticleEmitter::setNumToEmit(const FloatParticleParameter& numToEmit)
	{
		mNumToEmit = numToEmit;
	}

	void ParticleEmitter::setPosition(const Vec2ParticleParameter& position)
	{
		mPosition = position;
	}

	void ParticleEmitter::setVelocity(const Vec2ParticleParameter& velocity)
	{
		mVelocity = velocity;
	}

	void ParticleEmitter::setAcceleration(const Vec2ParticleParameter& acceleration)
	{
		mAcceleration = acceleration;
	}

	void ParticleEmitter::setStartColor(const Vec4ParticleParameter& startColor)
	{
		mStartColor = startColor;
	}

	void ParticleEmitter::setFinalColor(const Vec4ParticleParameter& finalColor)
	{
		mFinalColor = finalColor;
	}

	void ParticleEmitter::setStartSize(const FloatParticleParameter& startSize)
	{
		mStartSize = startSize;
	}

	void ParticleEmitter::setFinalSize(const FloatParticleParameter& finalSize)
	{
		mFinalSize = finalSize;
	}

	void ParticleEmitter::setStartRotation(const FloatParticleParameter& startRotation)
	{
		mStartRotation = startRotation;
	}

	void ParticleEmitter::setFinalRotation(const FloatParticleParameter& finalRotation)
	{
		mFinalRotation = finalRotation;
	}

	void ParticleEmitter::setDrag(const FloatParticleParameter& drag)
	{
		mDrag = drag;
	}

	void ParticleEmitter::setLifeSpan(const FloatParticleParameter& lifeSpan)
	{
		mLifeSpan = lifeSpan;
	}

	void ParticleEmitter::emitParticles(ParticlePool& particlePool)
	{		
		if (mTotalParticles == 0 || mNumRemaining > 0)
		{
			if (auto numToEmit = (uint32_t)getParameterValue(mNumToEmit); numToEmit > 0)
			{
				if (mTotalParticles > 0)
				{
					mNumRemaining -= numToEmit;
				}

				for (uint32_t idx = 0; idx < numToEmit; idx++)
				{
					auto& particleSprite = particlePool.getParticleSprite();
					initParticleSprite(particleSprite);
				}
			}
		}
	}

	void ParticleEmitter::initParticleSprite(Particle& particleSprite)
	{
		if (mTexture != nullptr)
		{
			particleSprite.setTexture(*mTexture, mSrcPosition, mSrcSize);
		}

		particleSprite.setPosition(getParameterValue(mPosition));
		particleSprite.setVelocity(getParameterValue(mVelocity));
		particleSprite.setAcceleration(getParameterValue(mAcceleration));
		particleSprite.setStartSize(getParameterValue(mStartSize));
		particleSprite.setFinalSize(getParameterValue(mFinalSize));
		particleSprite.setStartColor(getParameterValue(mStartColor));
		particleSprite.setFinalColor(getParameterValue(mFinalColor));
		particleSprite.setStartRotation(getParameterValue(mStartRotation));
		particleSprite.setFinalRotation(getParameterValue(mFinalRotation));
		particleSprite.setDrag(getParameterValue(mDrag));
		particleSprite.setLifeSpan(getParameterValue(mLifeSpan));
	}

	float ParticleEmitter::getParameterValue(const FloatParticleParameter& parameter)
	{
		switch (parameter.distributionType)
		{
		case DistributionType::Constant:
			return parameter.minValue;

		case DistributionType::Random:
			return mRandom.getFloat(parameter.minValue, parameter.maxValue);

		default:
			break;
		}

		return 0.0f;
	}

	glm::vec2 ParticleEmitter::getParameterValue(const Vec2ParticleParameter& parameter)
	{
		switch (parameter.distributionType)
		{
		case DistributionType::Constant:
			return parameter.minValue;

		case DistributionType::Random:
			return mRandom.getVec2(parameter.minValue, parameter.maxValue);

		default:
			break;
		}

		return glm::vec2{ 0.0f };
	}

	glm::vec4 ParticleEmitter::getParameterValue(const Vec4ParticleParameter& parameter)
	{
		switch (parameter.distributionType)
		{
		case DistributionType::Constant:
			return parameter.minValue;

		case DistributionType::Random:
			return mRandom.getVec4(parameter.minValue, parameter.maxValue);

		default:
			break;
		}

		return glm::vec4{ 0.0f };
	}

	void ParticleEmitterEditor::setParticleEmitter(ParticleEmitter& emitter)
	{
		mParticleEmitter = &emitter;
		if (mParticleEmitter->mTexture != nullptr)
		{
			mSelectedTextureFile = mParticleEmitter->mTexture->getFileName();
		}
	}

	void ParticleEmitterEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{
		if (layout.beginLayout("General"))
		{
			layout.inputString("Name", mParticleEmitter->mName);
			layout.inputUInt32("Total Particles", mParticleEmitter->mTotalParticles);

			if (layout.fileCombo("Texture", FileType::Texture, mSelectedTextureFile))
			{
				if (!mParticleEmitter->mTexture || mParticleEmitter->mTexture->getFileName() != mSelectedTextureFile)
				{
					if (mSelectedTextureFile.empty())
					{
						mParticleEmitter->mTexture = nullptr;
					}
					else
					{
						if (!cache.isLoaded<Texture>(mSelectedTextureFile))
						{
							auto texture = std::make_unique<Texture>();
							if (!texture->create(mSelectedTextureFile, wgpu::TextureFormat::RGBA8Unorm))
							{
								LogError("Texture::create() failed for: '%s'", mSelectedTextureFile.c_str());
							}
							else
							{
								cache.addResource(std::move(texture));
							}
						}

						mParticleEmitter->mTexture = cache.getResource<Texture>(mSelectedTextureFile);
					}
				}
			}

			layout.inputVec2("Src Position", mParticleEmitter->mSrcPosition);
			layout.inputVec2("Src Size", mParticleEmitter->mSrcSize);
			layout.endLayout();
		}

		parameterGui(layout, "Number To Emit", mParticleEmitter->mNumToEmit);
		parameterGui(layout, "Position", mParticleEmitter->mPosition);
		parameterGui(layout, "Velocity", mParticleEmitter->mVelocity);
		parameterGui(layout, "Acceleration", mParticleEmitter->mAcceleration);
		parameterGui(layout, "Start Size", mParticleEmitter->mStartSize);
		parameterGui(layout, "Final Size", mParticleEmitter->mFinalSize);
		parameterGui(layout, "Start Color", mParticleEmitter->mStartColor);
		parameterGui(layout, "Final Color", mParticleEmitter->mFinalColor);
		parameterGui(layout, "Start Rotation", mParticleEmitter->mStartRotation);
		parameterGui(layout, "Final Rotation", mParticleEmitter->mFinalRotation);
		parameterGui(layout, "Drag", mParticleEmitter->mDrag);
		parameterGui(layout, "Life Span", mParticleEmitter->mLifeSpan);
	}

	const char* ParticleEmitterEditor::getDistributionTypeStr(DistributionType distributionType) const
	{
		switch (distributionType)
		{
		case DistributionType::Constant:	return "Constant";			
		case DistributionType::Random:		return "Random";
		}

		return "";
	}

	void ParticleEmitterEditor::distributionTypeGui(const IEditorLayout& layout, DistributionType& distributionType)
	{
		if (layout.beginCombo("Type", getDistributionTypeStr(distributionType)))
		{
			if (layout.comboItem(getDistributionTypeStr(DistributionType::Constant), distributionType == DistributionType::Constant))
			{
				distributionType = DistributionType::Constant;
			}

			if (layout.comboItem(getDistributionTypeStr(DistributionType::Random), distributionType == DistributionType::Random))
			{
				distributionType = DistributionType::Random;
			}

			layout.endCombo();
		}
	}

	void ParticleEmitterEditor::parameterGui(const IEditorLayout& layout, const std::string& name, FloatParticleParameter& parameter)
	{
		if (layout.beginLayout(name))
		{
			distributionTypeGui(layout, parameter.distributionType);

			if (parameter.distributionType == DistributionType::Constant)
			{
				layout.inputFloat("Constant", parameter.minValue);
			}
			else
			{
				layout.inputFloat("Min Value", parameter.minValue);
				layout.inputFloat("Max Value", parameter.maxValue);
			}

			layout.endLayout();
		}
	}

	void ParticleEmitterEditor::parameterGui(const IEditorLayout& layout, const std::string& name, Vec2ParticleParameter& parameter)
	{
		if (layout.beginLayout(name))
		{
			distributionTypeGui(layout, parameter.distributionType);

			if (parameter.distributionType == DistributionType::Constant)
			{
				layout.inputVec2("Constant", parameter.minValue);
			}
			else
			{
				layout.inputVec2("Min Value", parameter.minValue);
				layout.inputVec2("Max Value", parameter.maxValue);
			}

			layout.endLayout();
		}
	}

	void ParticleEmitterEditor::parameterGui(const IEditorLayout& layout, const std::string& name, Vec4ParticleParameter& parameter)
	{
		if (layout.beginLayout(name))
		{
			distributionTypeGui(layout, parameter.distributionType);

			if (parameter.distributionType == DistributionType::Constant)
			{
				layout.inputVec4("Constant", parameter.minValue);
			}
			else
			{
				layout.inputVec4("Min Value", parameter.minValue);
				layout.inputVec4("Max Value", parameter.maxValue);
			}

			layout.endLayout();
		}
	}

	void ParticleEmitterSerializer::setParticleEmitter(ParticleEmitter& emitter)
	{
		mParticleEmitter = &emitter;
	}

	bool ParticleEmitterSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		mParticleEmitter->mName = reader.readString();

		if (!reader.read(&mParticleEmitter->mTotalParticles))
		{
			LogError("FileReader::read() failed for 'total particles'");
			return false;
		}

		auto textureFileName = reader.readString();
		if (!textureFileName.empty())
		{
			if (!cache.isLoaded<Texture>(textureFileName))
			{
				auto texture = std::make_unique<Texture>();
				if (!texture->create(textureFileName, wgpu::TextureFormat::RGBA8Unorm))
				{
					LogError("Texture::create() failed for: '%s'", textureFileName.c_str());
					return false;
				}
				else
				{
					cache.addResource(std::move(texture));
				}
			}

			mParticleEmitter->mTexture = cache.getResource<Texture>(textureFileName);
		}

		if (!reader.read(glm::value_ptr(mParticleEmitter->mSrcPosition)))
		{
			LogError("FileReader::read() failed for 'src position'");
			return false;
		}

		if (!reader.read(glm::value_ptr(mParticleEmitter->mSrcSize)))
		{
			LogError("FileReader::read() failed for 'src size'");
			return false;
		}

		if (!readParameter(reader, mParticleEmitter->mNumToEmit))
		{
			LogError("ParticleEmitterSerializer::readParameter() failed for 'num edit'");
			return false;
		}

		if (!readParameter(reader, mParticleEmitter->mPosition))
		{
			LogError("ParticleEmitterSerializer::readParameter() failed for 'position'");
			return false;
		}

		if (!readParameter(reader, mParticleEmitter->mVelocity))
		{
			LogError("ParticleEmitterSerializer::readParameter() failed for 'velocity'");
			return false;
		}

		if (!readParameter(reader, mParticleEmitter->mAcceleration))
		{
			LogError("ParticleEmitterSerializer::readParameter() failed for 'acceleration'");
			return false;
		}

		if (!readParameter(reader, mParticleEmitter->mStartSize))
		{
			LogError("ParticleEmitterSerializer::readParameter() failed for 'start size'");
			return false;
		}

		if (!readParameter(reader, mParticleEmitter->mFinalSize))
		{
			LogError("ParticleEmitterSerializer::readParameter() failed for 'final size'");
			return false;
		}

		if (!readParameter(reader, mParticleEmitter->mStartColor))
		{
			LogError("ParticleEmitterSerializer::readParameter() failed for 'start color'");
			return false;
		}

		if (!readParameter(reader, mParticleEmitter->mFinalColor))
		{
			LogError("ParticleEmitterSerializer::readParameter() failed for 'final color'");
			return false;
		}

		if (!readParameter(reader, mParticleEmitter->mStartRotation))
		{
			LogError("ParticleEmitterSerializer::readParameter() failed for 'start rotation'");
			return false;
		}

		if (!readParameter(reader, mParticleEmitter->mFinalRotation))
		{
			LogError("ParticleEmitterSerializer::readParameter() failed for 'final rotation'");
			return false;
		}

		if (!readParameter(reader, mParticleEmitter->mDrag))
		{
			LogError("ParticleEmitterSerializer::readParameter() failed for 'drag'");
			return false;
		}

		if (!readParameter(reader, mParticleEmitter->mLifeSpan))
		{
			LogError("ParticleEmitterSerializer::readParameter() failed for 'life span'");
			return false;
		}
		
		return true;
	}

	bool ParticleEmitterSerializer::write(FileWriter& writer)
	{
		if (!writer.writeString(mParticleEmitter->mName))
		{
			LogError("FileWriter::write() failed for 'name'");
			return false;
		}

		if (!writer.write(&mParticleEmitter->mTotalParticles))
		{
			LogError("FileWriter::write() failed for 'total particles'");
			return false;
		}

		std::string textureFileName;
		if (mParticleEmitter->mTexture != nullptr)
		{
			textureFileName = mParticleEmitter->mTexture->getFileName();
		}

		if (!writer.writeString(textureFileName))
		{
			LogError("FileWriter::writeString() failed for '%s'", textureFileName.c_str());
			return false;
		}

		if (!writer.write(glm::value_ptr(mParticleEmitter->mSrcPosition)))
		{
			LogError("FileWriter::write() failed for 'src position'");
			return false;
		}

		if (!writer.write(glm::value_ptr(mParticleEmitter->mSrcSize)))
		{
			LogError("FileWriter::write() failed for 'src size'");
			return false;
		}

		if (!writeParameter(writer, mParticleEmitter->mNumToEmit))
		{
			LogError("writeParameter() failed for 'num to emit'");
			return false;
		}

		if (!writeParameter(writer, mParticleEmitter->mPosition))
		{
			LogError("writeParameter() failed for 'position'");
			return false;
		}

		if (!writeParameter(writer, mParticleEmitter->mVelocity))
		{
			LogError("writeParameter() failed for 'velocity'");
			return false;
		}

		if (!writeParameter(writer, mParticleEmitter->mAcceleration))
		{
			LogError("writeParameter() failed for 'acceleration'");
			return false;
		}

		if (!writeParameter(writer, mParticleEmitter->mStartSize))
		{
			LogError("writeParameter() failed for 'start size'");
			return false;
		}

		if (!writeParameter(writer, mParticleEmitter->mFinalSize))
		{
			LogError("writeParameter() failed for 'final size'");
			return false;
		}

		if (!writeParameter(writer, mParticleEmitter->mStartColor))
		{
			LogError("writeParameter() failed for 'start color'");
			return false;
		}

		if (!writeParameter(writer, mParticleEmitter->mFinalColor))
		{
			LogError("writeParameter() failed for 'final color'");
			return false;
		}

		if (!writeParameter(writer, mParticleEmitter->mStartRotation))
		{
			LogError("writeParameter() failed for 'start rotation'");
			return false;
		}

		if (!writeParameter(writer, mParticleEmitter->mFinalRotation))
		{
			LogError("writeParameter() failed for 'final rotation'");
			return false;
		}

		if (!writeParameter(writer, mParticleEmitter->mDrag))
		{
			LogError("writeParameter() failed for 'drag'");
			return false;
		}

		if (!writeParameter(writer, mParticleEmitter->mLifeSpan))
		{
			LogError("writeParameter() failed for 'life span'");
			return false;
		}

		return true;
	}

	bool ParticleEmitterSerializer::read(json& object, ResourceCache& cache)
	{
		if (!object.contains("name"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'name' key");
			return false;
		}

		if (!object.contains("totalParticles"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'name' key");
			return false;
		}

		if (!object.contains("texture"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'texture' key");
			return false;
		}

		if (!object.contains("srcPosition"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'srcPosition' key");
			return false;
		}

		if (!object.contains("srcSize"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'srcSize' key");
			return false;
		}

		if (!object.contains("numToEmit"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'numToEmit' key");
			return false;
		}

		if (!object.contains("position"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'position' key");
			return false;
		}

		if (!object.contains("velocity"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'velocity' key");
			return false;
		}

		if (!object.contains("acceleration"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'acceleration' key");
			return false;
		}

		if (!object.contains("startSize"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'start size' key");
			return false;
		}

		if (!object.contains("finalSize"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'final size' key");
			return false;
		}

		if (!object.contains("startColor"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'start color' key");
			return false;
		}

		if (!object.contains("finalColor"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'final color' key");
			return false;
		}

		if (!object.contains("startRotation"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'start rotation' key");
			return false;
		}

		if (!object.contains("finalRotation"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'final rotation' key");
			return false;
		}

		if (!object.contains("drag"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'drag' key");
			return false;
		}

		if (!object.contains("lifeSpan"))
		{
			LogError("ParticleEmitter JSON object doesn't contain 'life span' key");
			return false;
		}

		mParticleEmitter->mName = object["name"].get<std::string>();
		mParticleEmitter->mTotalParticles = object["totalParticles"].get<uint32_t>();

		auto textureFileName = object["texture"].get<std::string>();
		if (!textureFileName.empty())
		{
			if (!cache.isLoaded<Texture>(textureFileName))
			{
				auto texture = std::make_unique<Texture>();
				if (!texture->create(textureFileName, wgpu::TextureFormat::RGBA8Unorm))
				{
					LogError("Texture::create() failed for: '%s'", textureFileName.c_str());
					return false;
				}
				else
				{
					cache.addResource(std::move(texture));
				}
			}

			mParticleEmitter->mTexture = cache.getResource<Texture>(textureFileName);
		}

		mParticleEmitter->mSrcPosition = {
			object["srcPosition"][0].get<float>(),
			object["srcPosition"][1].get<float>()
		};

		mParticleEmitter->mSrcSize = {
			object["srcSize"][0].get<float>(),
			object["srcSize"][1].get<float>()
		};

		if (!readParameter(object["numToEmit"], mParticleEmitter->mNumToEmit))
		{
			LogError("readParameter() failed for 'num to emit'");
			return false;
		}

		if (!readParameter(object["position"], mParticleEmitter->mPosition))
		{
			LogError("readParameter() failed for 'position'");
			return false;
		}

		if (!readParameter(object["velocity"], mParticleEmitter->mVelocity))
		{
			LogError("readParameter() failed for 'velocity'");
			return false;
		}

		if (!readParameter(object["acceleration"], mParticleEmitter->mAcceleration))
		{
			LogError("readParameter() failed for 'acceleration'");
			return false;
		}

		if (!readParameter(object["startSize"], mParticleEmitter->mStartSize))
		{
			LogError("readParameter() failed for 'start size'");
			return false;
		}

		if (!readParameter(object["finalSize"], mParticleEmitter->mFinalSize))
		{
			LogError("readParameter() failed for 'final size'");
			return false;
		}

		if (!readParameter(object["startColor"], mParticleEmitter->mStartColor))
		{
			LogError("readParameter() failed for 'start color'");
			return false;
		}

		if (!readParameter(object["finalColor"], mParticleEmitter->mFinalColor))
		{
			LogError("readParameter() failed for 'final color'");
			return false;
		}

		if (!readParameter(object["startRotation"], mParticleEmitter->mStartRotation))
		{
			LogError("readParameter() failed for 'start rotation'");
			return false;
		}

		if (!readParameter(object["finalRotation"], mParticleEmitter->mFinalRotation))
		{
			LogError("readParameter() failed for 'final rotation'");
			return false;
		}

		if (!readParameter(object["drag"], mParticleEmitter->mDrag))
		{
			LogError("readParameter() failed for 'drag'");
			return false;
		}

		if (!readParameter(object["lifeSpan"], mParticleEmitter->mLifeSpan))
		{
			LogError("readParameter() failed for 'life span'");
			return false;
		}

		return true;
	}

	bool ParticleEmitterSerializer::write(json& object)
	{
		object["name"] = mParticleEmitter->mName;
		object["totalParticles"] = mParticleEmitter->mTotalParticles;

		std::string textureFileName;
		if (mParticleEmitter->mTexture != nullptr)
		{
			textureFileName = mParticleEmitter->mTexture->getFileName();
		}

		object["texture"] = textureFileName;
		object["srcPosition"] = std::vector<float>{
			mParticleEmitter->mSrcPosition.x,
			mParticleEmitter->mSrcPosition.y
		};

		object["srcSize"] = std::vector<float>{
			mParticleEmitter->mSrcSize.x,
			mParticleEmitter->mSrcSize.y
		};

		if (!writeParameter(object["numToEmit"], mParticleEmitter->mNumToEmit))
		{
			LogError("writeParameter() failed for 'num to emit'");
			return false;
		}

		if (!writeParameter(object["position"], mParticleEmitter->mPosition))
		{
			LogError("writeParameter() failed for 'position'");
			return false;
		}

		if (!writeParameter(object["velocity"], mParticleEmitter->mVelocity))
		{
			LogError("writeParameter() failed for 'velocity'");
			return false;
		}

		if (!writeParameter(object["acceleration"], mParticleEmitter->mAcceleration))
		{
			LogError("writeParameter() failed for 'acceleration'");
			return false;
		}

		if (!writeParameter(object["startSize"], mParticleEmitter->mStartSize))
		{
			LogError("writeParameter() failed for 'start size'");
			return false;
		}

		if (!writeParameter(object["finalSize"], mParticleEmitter->mFinalSize))
		{
			LogError("writeParameter() failed for 'final size'");
			return false;
		}

		if (!writeParameter(object["startColor"], mParticleEmitter->mStartColor))
		{
			LogError("writeParameter() failed for 'start color'");
			return false;
		}

		if (!writeParameter(object["finalColor"], mParticleEmitter->mFinalColor))
		{
			LogError("writeParameter() failed for 'final color'");
			return false;
		}

		if (!writeParameter(object["startRotation"], mParticleEmitter->mStartRotation))
		{
			LogError("writeParameter() failed for 'start rotation'");
			return false;
		}

		if (!writeParameter(object["finalRotation"], mParticleEmitter->mFinalRotation))
		{
			LogError("writeParameter() failed for 'final rotation'");
			return false;
		}

		if (!writeParameter(object["drag"], mParticleEmitter->mDrag))
		{
			LogError("writeParameter() failed for 'drag'");
			return false;
		}

		if (!writeParameter(object["lifeSpan"], mParticleEmitter->mLifeSpan))
		{
			LogError("writeParameter() failed for 'life span'");
			return false;
		}

		return true;
	}

	bool ParticleEmitterSerializer::readParameter(FileReader& reader, FloatParticleParameter& parameter)
	{
		uint32_t distributionType{ 0 };
		if (!reader.read(&distributionType))
		{
			LogError("FileReader::read() failed for 'distribution type'");
			return false;
		}

		parameter.distributionType = (DistributionType)distributionType;

		if (!reader.read(&parameter.minValue))
		{
			LogError("FileReader::read() failed for 'min value'");
			return false;
		}

		if (!reader.read(&parameter.maxValue))
		{
			LogError("FileReader::read() failed for 'max value'");
			return false;
		}

		return true;
	}

	bool ParticleEmitterSerializer::readParameter(FileReader& reader, Vec2ParticleParameter& parameter)
	{
		uint32_t distributionType{ 0 };
		if (!reader.read(&distributionType))
		{
			LogError("FileReader::read() failed for 'distribution type'");
			return false;
		}

		parameter.distributionType = (DistributionType)distributionType;

		if (!reader.read(glm::value_ptr(parameter.minValue)))
		{
			LogError("FileReader::read() failed for 'min value'");
			return false;
		}

		if (!reader.read(glm::value_ptr(parameter.maxValue)))
		{
			LogError("FileReader::read() failed for 'max value'");
			return false;
		}

		return true;
	}

	bool ParticleEmitterSerializer::readParameter(FileReader& reader, Vec4ParticleParameter& parameter)
	{
		uint32_t distributionType{ 0 };
		if (!reader.read(&distributionType))
		{
			LogError("FileReader::read() failed for 'distribution type'");
			return false;
		}

		parameter.distributionType = (DistributionType)distributionType;

		if (!reader.read(glm::value_ptr(parameter.minValue)))
		{
			LogError("FileReader::read() failed for 'min value'");
			return false;
		}

		if (!reader.read(glm::value_ptr(parameter.maxValue)))
		{
			LogError("FileReader::read() failed for 'max value'");
			return false;
		}

		return true;
	}

	bool ParticleEmitterSerializer::writeParameter(FileWriter& writer, const FloatParticleParameter& parameter)
	{
		auto distributionType = (uint32_t)parameter.distributionType;
		if (!writer.write(&distributionType))
		{
			LogError("FileWriter::write() failed for 'distribution type'");
			return false;
		}

		if (!writer.write(&parameter.minValue))
		{
			LogError("FileWriter::write() failed for 'min value'");
			return false;
		}

		if (!writer.write(&parameter.maxValue))
		{
			LogError("FileWriter::write() failed for 'max value'");
			return false;
		}

		return true;
	}

	bool ParticleEmitterSerializer::writeParameter(FileWriter& writer, const Vec2ParticleParameter& parameter)
	{
		auto distributionType = (uint32_t)parameter.distributionType;
		if (!writer.write(&distributionType))
		{
			LogError("FileWriter::write() failed for 'distribution type'");
			return false;
		}

		if (!writer.write(glm::value_ptr(parameter.minValue)))
		{
			LogError("FileWriter::write() failed for 'min value'");
			return false;
		}

		if (!writer.write(glm::value_ptr(parameter.maxValue)))
		{
			LogError("FileWriter::write() failed for 'max value'");
			return false;
		}

		return true;
	}

	bool ParticleEmitterSerializer::writeParameter(FileWriter& writer, const Vec4ParticleParameter& parameter)
	{
		auto distributionType = (uint32_t)parameter.distributionType;
		if (!writer.write(&distributionType))
		{
			LogError("FileWriter::write() failed for 'distribution type'");
			return false;
		}

		if (!writer.write(glm::value_ptr(parameter.minValue)))
		{
			LogError("FileWriter::write() failed for 'min value'");
			return false;
		}

		if (!writer.write(glm::value_ptr(parameter.maxValue)))
		{
			LogError("FileWriter::write() failed for 'max value'");
			return false;
		}

		return true;
	}

	bool ParticleEmitterSerializer::readParameter(const json& object, FloatParticleParameter& parameter)
	{
		if (!object.contains("distributionType"))
		{
			LogError("Parameter JSON object doesn't contain 'distribution type' key");
			return false;
		}

		if (!object.contains("minValue"))
		{
			LogError("Parameter JSON object doesn't contain 'min value' key");
			return false;
		}

		if (!object.contains("maxValue"))
		{
			LogError("Parameter JSON object doesn't contain 'max value' key");
			return false;
		}

		parameter.distributionType = (DistributionType)object["distributionType"].get<uint32_t>();
		parameter.minValue = object["minValue"].get<float>();
		parameter.maxValue = object["maxValue"].get<float>();

		return true;
	}

	bool ParticleEmitterSerializer::readParameter(const json& object, Vec2ParticleParameter& parameter)
	{
		if (!object.contains("distributionType"))
		{
			LogError("Parameter JSON object doesn't contain 'distribution type' key");
			return false;
		}

		if (!object.contains("minValue"))
		{
			LogError("Parameter JSON object doesn't contain 'min value' key");
			return false;
		}

		if (!object.contains("maxValue"))
		{
			LogError("Parameter JSON object doesn't contain 'max value' key");
			return false;
		}

		parameter.distributionType = (DistributionType)object["distributionType"].get<uint32_t>();
		parameter.minValue = {
			object["minValue"][0].get<float>(),
			object["minValue"][1].get<float>()
		};

		parameter.maxValue = {
			object["maxValue"][0].get<float>(),
			object["maxValue"][1].get<float>()
		};

		return true;
	}

	bool ParticleEmitterSerializer::readParameter(const json& object, Vec4ParticleParameter& parameter)
	{
		if (!object.contains("distributionType"))
		{
			LogError("Parameter JSON object doesn't contain 'distribution type' key");
			return false;
		}

		if (!object.contains("minValue"))
		{
			LogError("Parameter JSON object doesn't contain 'min value' key");
			return false;
		}

		if (!object.contains("maxValue"))
		{
			LogError("Parameter JSON object doesn't contain 'max value' key");
			return false;
		}

		parameter.distributionType = (DistributionType)object["distributionType"].get<uint32_t>();
		parameter.minValue = {
			object["minValue"][0].get<float>(),
			object["minValue"][1].get<float>(),
			object["minValue"][2].get<float>(),
			object["minValue"][3].get<float>()
		};

		parameter.maxValue = {
			object["maxValue"][0].get<float>(),
			object["maxValue"][1].get<float>(),
			object["maxValue"][2].get<float>(),
			object["maxValue"][3].get<float>()
		};

		return true;
	}

	bool ParticleEmitterSerializer::writeParameter(json& object, const FloatParticleParameter& parameter)
	{
		object["distributionType"] = (uint32_t)parameter.distributionType;
		object["minValue"] = parameter.minValue;
		object["maxValue"] = parameter.maxValue;

		return true;
	}

	bool ParticleEmitterSerializer::writeParameter(json& object, const Vec2ParticleParameter& parameter)
	{
		object["distributionType"] = (uint32_t)parameter.distributionType;
		object["minValue"] = std::vector<float>{
			parameter.minValue.x,
			parameter.minValue.y
		};

		object["maxValue"] = std::vector<float>{
			parameter.maxValue.x,
			parameter.maxValue.y
		};

		return true;
	}

	bool ParticleEmitterSerializer::writeParameter(json& object, const Vec4ParticleParameter& parameter)
	{
		object["distributionType"] = (uint32_t)parameter.distributionType;
		object["minValue"] = std::vector<float>{
			parameter.minValue.x,
			parameter.minValue.y,
			parameter.minValue.z,
			parameter.minValue.w
		};

		object["maxValue"] = std::vector<float>{
			parameter.maxValue.x,
			parameter.maxValue.y,
			parameter.minValue.z,
			parameter.minValue.w
		};

		return true;
	}
}
