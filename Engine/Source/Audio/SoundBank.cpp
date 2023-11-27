#include "Audio/SoundBank.h"
#include "Audio/AudioSystem.h"
#include "VFS/FileSystem.h"
#include "Core/Logger.h"
#include "fmod_errors.h"

namespace Trinity
{	
	SoundBank::~SoundBank()
	{
		destroy();
	}

	bool SoundBank::create(const std::string& fileName)
	{
		auto file = FileSystem::get().openFile(fileName, FileOpenMode::OpenRead);
		if (!file)
		{
			LogError("FileSystem::openFile() failed for: %s", fileName.c_str());
			return false;
		}

		FileReader reader(*file);
		std::vector<char> buffer(reader.getSize());
		reader.read(buffer.data(), reader.getSize());

		FMOD::Studio::System* system = AudioSystem::get();
		FMOD_RESULT result = system->loadBankMemory(
			buffer.data(), 
			reader.getSize(), 
			FMOD_STUDIO_LOAD_MEMORY, 
			FMOD_STUDIO_LOAD_BANK_NORMAL, 
			&mHandle
		);

		if (result != FMOD_OK)
		{
			LogError("System::loadBankMemory() failed for bank: '%s' with error: '%s'", 
				fileName.c_str(), FMOD_ErrorString(result));

			return false;
		}

		result = mHandle->loadSampleData();
		if (result != FMOD_OK)
		{
			LogError("Bank::loadSampleData() failed for bank: '%s' with error: '%s'", 
				fileName.c_str(), FMOD_ErrorString(result));

			return false;
		}

		int32_t numEvents{ 0 };
		mHandle->getEventCount(&numEvents);

		if (numEvents > 0)
		{
			std::vector<FMOD::Studio::EventDescription*> events(numEvents);
			mHandle->getEventList(events.data(), numEvents, &numEvents);

			char eventName[kMaxPathLen];
			for (auto* evt : events)
			{
				evt->getPath(eventName, kMaxPathLen, nullptr);
				mEvents.emplace(eventName, evt);
			}
		}

		int32_t numBuses{ 0 };
		mHandle->getBusCount(&numBuses);

		if (numBuses > 0)
		{
			std::vector<FMOD::Studio::Bus*> buses(numBuses);
			mHandle->getBusList(buses.data(), numBuses, &numBuses);

			char busName[kMaxPathLen];
			for (auto* bus : buses)
			{
				bus->getPath(busName, kMaxPathLen, nullptr);
				mBuses.emplace(busName, bus);
			}
		}

		return true;
	}

	void SoundBank::destroy()
	{
		mEvents.clear();
		mBuses.clear();

		if (mHandle != nullptr)
		{
			mHandle->unloadSampleData();
			mHandle->unload();
			mHandle = nullptr;
		}
	}

	bool SoundBank::hasEvent(const std::string& name) const
	{
		return mEvents.contains(name);
	}

	bool SoundBank::hasBus(const std::string& name) const
	{
		return mBuses.contains(name);
	}

	FMOD::Studio::EventDescription* SoundBank::getEvent(const std::string& name) const
	{
		if (auto it = mEvents.find(name); it != mEvents.end())
		{
			return it->second;
		}

		return nullptr;
	}

	FMOD::Studio::Bus* SoundBank::getBus(const std::string& name) const
	{
		if (auto it = mBuses.find(name); it != mBuses.end())
		{
			return it->second;
		}

		return nullptr;
	}

	float SoundBank::getBusVolume(const std::string& name) const
	{
		float retVal{ 0.0f };
		if (auto it = mBuses.find(name); it != mBuses.end())
		{
			it->second->getVolume(&retVal);
		}

		return retVal;
	}

	bool SoundBank::isBusPaused(const std::string& name) const
	{
		bool retVal{ 0.0f };
		if (auto it = mBuses.find(name); it != mBuses.end())
		{
			it->second->getPaused(&retVal);
		}

		return retVal;
	}

	void SoundBank::setBusVolume(const std::string& name, float volume)
	{
		if (auto it = mBuses.find(name); it != mBuses.end())
		{
			it->second->setVolume(volume);
		}
	}

	void SoundBank::setBusPaused(const std::string& name, bool paused)
	{
		if (auto it = mBuses.find(name); it != mBuses.end())
		{
			it->second->setPaused(paused);
		}
	}
}