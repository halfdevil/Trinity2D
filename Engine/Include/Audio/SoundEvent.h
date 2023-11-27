#pragma once

#include <string>
#include "studio/fmod_studio.hpp"

namespace Trinity
{
	class SoundEvent
	{
	public:

		SoundEvent() = default;
		virtual ~SoundEvent();

		SoundEvent(const SoundEvent&) = delete;
		SoundEvent& operator = (const SoundEvent&) = delete;

		SoundEvent(SoundEvent&&) noexcept = default;
		SoundEvent& operator = (SoundEvent&&) noexcept = default;

		FMOD::Studio::EventInstance* getHandle() const
		{
			return mHandle;
		}

		operator FMOD::Studio::EventInstance* () const
		{
			return mHandle;
		}

		virtual bool create(FMOD::Studio::EventDescription* description);
		virtual void destroy();

		virtual void start();
		virtual void restart();
		virtual void stop(bool allowFade = true);

		virtual bool isPaused() const;
		virtual float getVolume() const;
		virtual float getPitch() const;
		virtual float getParameter(const std::string& name) const;

		virtual void setPaused(bool paused);
		virtual void setVolume(float volume);
		virtual void setPitch(float pitch);
		virtual void setParameter(const std::string& name, float value);

	protected:

		FMOD::Studio::EventInstance* mHandle{ nullptr };
	};
}