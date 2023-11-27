#pragma once

#include "Core/Singleton.h"
#include "studio/fmod_studio.hpp"
#include <string>
#include <unordered_map>
#include <memory>

namespace Trinity
{
	class SoundBank;
	class SoundEvent;

	class AudioSystem : public Singleton<AudioSystem>
	{
	public:

		AudioSystem() = default;
		~AudioSystem();

		AudioSystem(const AudioSystem&) = delete;
		AudioSystem& operator = (const AudioSystem&) = delete;

		AudioSystem(AudioSystem&&) noexcept = default;
		AudioSystem& operator = (AudioSystem&&) noexcept = default;

		FMOD::Studio::System* getHandle() const
		{
			return mHandle;
		}

		operator FMOD::Studio::System* () const
		{
			return mHandle;
		}

		virtual bool create(uint32_t maxConcurrentSounds);
		virtual void destroy();

		virtual std::unique_ptr<SoundEvent> createEvent(const std::string& eventName);

		virtual SoundBank* getBank(const std::string& bankFile);
		virtual SoundBank* loadBank(const std::string& bankFile);
		virtual void unloadBank(const std::string& bankFile);

		virtual float getBusVolume(const std::string& name) const;
		virtual bool isBusPaused(const std::string& name) const;
		virtual void setBusVolume(const std::string& name, float volume);
		virtual void setBusPaused(const std::string& name, bool paused);

	protected:

		FMOD::Studio::System* mHandle{ nullptr };
		std::unordered_map<std::string, std::unique_ptr<SoundBank>> mBanks;
	};
}