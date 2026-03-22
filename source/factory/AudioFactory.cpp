#include "pch.h"
#include "AudioFactory.h"
#include <iostream>

namespace AudioFactory
{


	void AudioFactory::init()
	{
		this->bgm.add(false, "../../Assets/audio/battle.mp3");
		std::cout << "AUDIO INITIALIZE" << std::endl;
	}
	AudioFactory::AudioFactory() : 
		sfx{ 0.5f, 1.f, 0}, 
		amb{ 0.5f, 1.f, static_cast<s32>(-1)}, 
		bgm{ 0.5f, 1.f, static_cast<s32>(-1)}
	{
	}

	AudioFactory::AudioObject::AudioObject() : 
		volume{}, 
		pitch{}, 
		loop{}, 
		data{}
	{
		this->group = AEAudioCreateGroup();
		std::cout << "Is Group valid?: " << AEAudioIsValidGroup(this->group) << '\n';

	}

	AudioFactory::AudioObject::AudioObject(f32 volume, f32 pitch, s32 loop)
	{
		this->group = AEAudioCreateGroup();

		this->volume = volume;
		this->pitch = pitch;
		this->loop = loop;
		AEAudioSetGroupVolume(this->group, volume);
		AEAudioSetGroupPitch(this->group, pitch);

		std::cout << "Is Group valid?: " << AEAudioIsValidGroup(this->group) << "/n";
	}


	void AudioFactory::AudioObject::setVolume(f32 volume)
	{
		if (0.f <= volume && volume <= 1.f)
		{
			this->volume = volume;
			AEAudioSetGroupVolume(this->group, volume);
		}
	}

	void AudioFactory::AudioObject::setPitch(f32 pitch)
	{
		if (0.f <= pitch && pitch <= 1.f)
		{
			this->pitch = pitch;
			AEAudioSetGroupVolume(this->group, pitch);
		}
	}
	void AudioFactory::AudioObject::free()
	{
		for (AEAudio a : this->data)
		{
			AEAudioUnloadAudio(a);
		}
		AEAudioUnloadAudioGroup(this->group);
	}

	void AudioFactory::AudioObject::pause()
	{
		AEAudioPauseGroup(this->group);
	}
	void AudioFactory::AudioObject::resume()
	{
		AEAudioResumeGroup(this->group);
	}
	void AudioFactory::AudioObject::stop()
	{
		AEAudioStopGroup(this->group);
	}
	void AudioFactory::AudioObject::add(bool sfx, const char* path)
	{
		AEAudio buffer = AEAudioLoadSound(path);
		std::cout << "Is Audio valid?: " << AEAudioIsValidAudio(buffer) << std::endl;
		if (sfx) this->data.push_back(buffer);
		else this->data.push_back(buffer);
	}
	void AudioFactory::AudioObject::play(s32 index)
	{
		AEAudio buffer = this->data[index];
		std::cout << "Is Audio valid?: " << AEAudioIsValidAudio(buffer) << std::endl;
		std::cout << "Play Audio" << std::endl;

		AEAudioPlay(buffer, this->group, this->volume, this->pitch, this->loop);

	}

	void AudioFactory::free()
	{
		this->amb.free();
		this->sfx.free();
		this->bgm.free();
	}

}