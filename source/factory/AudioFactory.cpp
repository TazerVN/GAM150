#include "pch.h"
#include "AudioFactory.h"
#include <iostream>

namespace AudioFactory
{


	void AudioFactory::init()
	{
		this->bgm.add(false, "Assets/audio/battle.mp3");

		this->sfx.add(true, "Assets/audio/sfx/card.mp3");
		this->sfx.add(true, "Assets/audio/sfx/click.wav");
		this->sfx.add(true, "Assets/audio/sfx/attack.wav");
		this->sfx.add(true, "Assets/audio/sfx/attack2.wav");
		this->sfx.add(true, "Assets/audio/sfx/shoot.wav");
		this->sfx.add(true, "Assets/audio/sfx/damage.wav");
		this->sfx.add(true, "Assets/audio/sfx/damage2.wav");
		this->sfx.add(true, "Assets/audio/sfx/laser.wav");   // 7
		this->sfx.add(true, "Assets/audio/sfx/sniper.wav");  // 8

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
		stopping = false;
	}

	bool AudioFactory::AudioObject::isStop()
	{
		return stopping;
	}

	void AudioFactory::AudioObject::resetAudio()
	{
		stopping = false;
	}
	void AudioFactory::AudioObject::add(bool sfx, const char* path)
	{
		AEAudio buffer = AEAudioLoadSound(path);
		if (sfx) this->data.push_back(buffer);
		else this->data.push_back(buffer);
	}
	void AudioFactory::AudioObject::play(s32 index)
	{
		AEAudio buffer = this->data[index];

		if (!stopping) 
		{
			AEAudioPlay(buffer, this->group, this->volume, this->pitch, this->loop);
			stopping = true;
		}

	}

	void AudioFactory::free()
	{
		this->amb.free();
		this->sfx.free();
		this->bgm.free();
	}

}