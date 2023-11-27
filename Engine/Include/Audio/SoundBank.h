#pragma once

#include "fmod_studio.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace Trinity
{
	class SoundBank 
	{
	public:

		constexpr static const uint32_t kMaxPathLen = 512;

		SoundBank() = default;
		virtual ~SoundBank();

		SoundBank(const SoundBank&) = delete;
		SoundBank& operator = (const SoundBank&) = delete;

		SoundBank(SoundBank&&) noexcept = default;
		SoundBank& operator = (SoundBank&&) noexcept = default;

		FMOD::Studio::Bank* getHandle() const
		{
			return mHandle;
		}

		operator FMOD::Studio::Bank* () const
		{
			return mHandle;
		}

		virtual bool create(const std::string& fileName);
		virtual void destroy();

		virtual bool hasEvent(const std::string& name) const;
		virtual bool hasBus(const std::string& name) const;

		virtual FMOD::Studio::EventDescription* getEvent(const std::string& name) const;
		virtual FMOD::Studio::Bus* getBus(const std::string& name) const;

		virtual float getBusVolume(const std::string& name) const;
		virtual bool isBusPaused(const std::string& name) const;
		virtual void setBusVolume(const std::string& name, float volume);
		virtual void setBusPaused(const std::string& name, bool paused);

	protected:

		FMOD::Studio::Bank* mHandle{ nullptr };
		std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;
		std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
	};
}