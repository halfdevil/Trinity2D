#include "Audio/AudioSystem.h"
#include "Audio/SoundBank.h"
#include "Audio/SoundEvent.h"
#include "Core/Logger.h"
#include "fmod_errors.h"

namespace Trinity
{
	AudioSystem::~AudioSystem()
	{
		destroy();
	}

	bool AudioSystem::create(uint32_t maxConcurrentSounds)
	{
#ifdef DEBUG_BUILD
		FMOD::Debug_Initialize(
			FMOD_DEBUG_LEVEL_ERROR,
			FMOD_DEBUG_MODE_TTY
		);
#endif

		FMOD_RESULT result = FMOD::Studio::System::create(&mHandle);
		if (result != FMOD_OK)
		{
			LogError("System::create() failed with error: %s", FMOD_ErrorString(result));
			return false;
		}

		result = mHandle->initialize(
			maxConcurrentSounds,
			FMOD_STUDIO_INIT_NORMAL,
			FMOD_INIT_NORMAL,
			nullptr
		);

		if (result != FMOD_OK)
		{
			LogError("System::initialize() failed with error: %s", FMOD_ErrorString(result));
			return false;
		}

		return true;
	}

	void AudioSystem::destroy()
	{
		if (mHandle != nullptr)
		{
			mHandle->release();
			mHandle = nullptr;
		}
	}

	std::unique_ptr<SoundEvent> AudioSystem::createEvent(const std::string& eventName)
	{
		for (auto& it : mBanks)
		{
			if (it.second->hasEvent(eventName))
			{
				auto soundEvent = std::make_unique<SoundEvent>();
				if (!soundEvent->create(it.second->getEvent(eventName)))
				{
					LogError("SoundEvent::create() failed for: '%s'", eventName.c_str());
					return nullptr;
				}

				return soundEvent;
			}
		}

		return nullptr;
	}

	SoundBank* AudioSystem::getBank(const std::string& bankFile)
	{
		if (auto it = mBanks.find(bankFile); it != mBanks.end())
		{
			return it->second.get();
		}

		return nullptr;
	}

	SoundBank* AudioSystem::loadBank(const std::string& bankFile)
	{
		if (auto it = mBanks.find(bankFile); it != mBanks.end())
		{
			return it->second.get();
		}

		auto soundBank = std::make_unique<SoundBank>();
		if (soundBank->create(bankFile))
		{
			LogError("SoundBank::create() failed for: '%s'", bankFile.c_str());
			return nullptr;
		}

		auto soundBankPtr = soundBank.get();
		mBanks.emplace(bankFile, std::move(soundBank));

		return soundBankPtr;
	}

	void AudioSystem::unloadBank(const std::string& bankFile)
	{
		if (auto it = mBanks.find(bankFile); it != mBanks.end())
		{
			mBanks.erase(it);
		}
	}

	float AudioSystem::getBusVolume(const std::string& name) const
	{
		for (auto& it : mBanks)
		{
			if (it.second->hasBus(name))
			{
				return it.second->getBusVolume(name);
			}
		}

		return 0.0f;
	}

	bool AudioSystem::isBusPaused(const std::string& name) const
	{
		for (auto& it : mBanks)
		{
			if (it.second->hasBus(name))
			{
				return it.second->isBusPaused(name);
			}
		}

		return false;
	}

	void AudioSystem::setBusVolume(const std::string& name, float volume)
	{
		for (auto& it : mBanks)
		{
			if (it.second->hasBus(name))
			{
				it.second->setBusVolume(name, volume);
				break;
			}
		}
	}

	void AudioSystem::setBusPaused(const std::string& name, bool paused)
	{
		for (auto& it : mBanks)
		{
			if (it.second->hasBus(name))
			{
				it.second->setBusPaused(name, paused);
				break;
			}
		}
	}
}