#include "Audio/SoundEvent.h"
#include "Core/Logger.h"
#include "fmod_errors.h"

namespace Trinity
{
	SoundEvent::~SoundEvent()
	{
		destroy();
	}

	bool SoundEvent::create(FMOD::Studio::EventDescription* description)
	{
		FMOD_RESULT result = description->createInstance(&mHandle);
		if (result != FMOD_OK)
		{
			LogError("EventDescription::createInstance() failed for with error: %s", FMOD_ErrorString(result));
			return false;
		}

		return true;
	}

	void SoundEvent::destroy()
	{
		if (mHandle != nullptr)
		{
			mHandle->release();
			mHandle = nullptr;
		}
	}

	void SoundEvent::start()
	{
		if (mHandle != nullptr)
		{
			mHandle->start();
		}
	}

	void SoundEvent::restart()
	{
		if (mHandle != nullptr)
		{
			mHandle->stop(FMOD_STUDIO_STOP_IMMEDIATE);
			mHandle->start();
		}
	}

	void SoundEvent::stop(bool allowFade)
	{
		if (mHandle != nullptr)
		{
			mHandle->stop(allowFade ? FMOD_STUDIO_STOP_ALLOWFADEOUT : 
				FMOD_STUDIO_STOP_IMMEDIATE);
		}
	}

	bool SoundEvent::isPaused() const
	{
		bool retVal{ false };
		if (mHandle != nullptr)
		{
			mHandle->getPaused(&retVal);
		}

		return retVal;
	}

	float SoundEvent::getVolume() const
	{
		float retVal{ 0.0f };
		if (mHandle != nullptr)
		{
			mHandle->getVolume(&retVal);
		}

		return retVal;
	}

	float SoundEvent::getPitch() const
	{
		float retVal{ 0.0f };
		if (mHandle != nullptr)
		{
			mHandle->getPitch(&retVal);
		}

		return retVal;
	}

	float SoundEvent::getParameter(const std::string& name) const
	{
		float retVal{ 0.0f };
		if (mHandle != nullptr)
		{
			mHandle->getParameterByName(name.c_str(), &retVal);
		}

		return retVal;
	}

	void SoundEvent::setPaused(bool paused)
	{
		if (mHandle != nullptr)
		{
			mHandle->setPaused(paused);
		}
	}

	void SoundEvent::setVolume(float volume)
	{
		if (mHandle != nullptr)
		{
			mHandle->setVolume(volume);
		}
	}

	void SoundEvent::setPitch(float pitch)
	{
		if (mHandle != nullptr)
		{
			mHandle->setPitch(pitch);
		}
	}

	void SoundEvent::setParameter(const std::string& name, float value)
	{
		if (mHandle != nullptr)
		{
			mHandle->setParameterByName(name.c_str(), value);
		}
	}
}