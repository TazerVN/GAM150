//=========================================
//	AUTHOR:		PHAM MINH TUAN
// 
//	EMAIL:		minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//=========================================

#include "pch.h"
#include "AudioFactory.h"
#include <iostream>

namespace AudioFactory
{


	void AudioFactory::init()
	{
		this->bgm.add(false, "Assets/audio/music/battle.mp3");
		this->bgm.add(false, "Assets/audio/music/menu.mp3");
		this->bgm.add(false, "Assets/audio/music/gameover.mp3");

		this->sfx.add(true, "Assets/audio/sfx/card.mp3");
		this->sfx.add(true, "Assets/audio/sfx/click.mp3");
		this->sfx.add(true, "Assets/audio/sfx/attack.mp3");
		this->sfx.add(true, "Assets/audio/sfx/attack2.mp3");
		this->sfx.add(true, "Assets/audio/sfx/shoot.mp3");
		this->sfx.add(true, "Assets/audio/sfx/damage.mp3");
		this->sfx.add(true, "Assets/audio/sfx/damage2.mp3");
		this->sfx.add(true, "Assets/audio/sfx/laser.mp3");   // 7
		this->sfx.add(true, "Assets/audio/sfx/sniper.mp3");  // 8
		this->sfx.add(true, "Assets/audio/sfx/select.mp3");  // 9
		this->sfx.add(true, "Assets/audio/sfx/click2.mp3");  // 10
		this->sfx.add(true, "Assets/audio/sfx/miss.mp3");  // 11

		std::cout << "AUDIO INITIALIZE" << '\n';
	}
	AudioFactory::AudioFactory() : 
		sfx{ gameData.soundSettings.sfx, 1.f, 0}, 
		amb{ gameData.soundSettings.ambience, 1.f, static_cast<s32>(-1)}, 
		bgm{ gameData.soundSettings.music, 1.f, static_cast<s32>(-1)}
	{
	}

	AudioFactory::AudioObject::AudioObject() : 
		volume{}, 
		pitch{}, 
		loop{}, 
		data{},
		og_volume{}
	{
		this->group = AEAudioCreateGroup();
		std::cout << "Is Group valid?: " << AEAudioIsValidGroup(this->group) << '\n';

	}

	AudioFactory::AudioObject::AudioObject(f32 volume, f32 pitch, s32 loop)
	{
		this->group = AEAudioCreateGroup();

		this->og_volume = volume;
		this->volume = volume;
		this->pitch = pitch;
		this->loop = loop;
		AEAudioSetGroupVolume(this->group, volume);
		AEAudioSetGroupPitch(this->group, pitch);

		std::cout << "Is Group valid?: " << AEAudioIsValidGroup(this->group) << "/n";
	}


	void AudioFactory::AudioObject::setVolume(f32 _volume)
	{
		if (0.f <= _volume && _volume <= 1.f)
		{
			this->volume = _volume;
			this->og_volume = _volume;
			AEAudioSetGroupVolume(this->group, _volume);
		}
	}

	void AudioFactory::AudioObject::setPitch(f32 _pitch)
	{
		if (0.f <= _pitch && _pitch <= 1.f)
		{
			this->pitch = _pitch;
			AEAudioSetGroupVolume(this->group, _pitch);
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