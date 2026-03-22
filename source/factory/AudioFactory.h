#pragma once

#include "AEEngine.h"
#include <vector>

namespace AudioFactory
{
	class AudioFactory
	{
		public:

		struct AudioObject
		{
			AEAudioGroup group;
			std::vector<AEAudio> data;
			f32 volume;
			f32 pitch;
			s32 loop;
			void setVolume(f32 volume);
			void setPitch(f32 volume);
			void pause();
			void resume();
			void stop();
			void free();
			void add(bool sfx, const char* path);
			void play(s32 index);
			AudioObject();
			AudioObject(f32 volume, f32 pitch, s32 loop);
		};

		AudioObject bgm;
		AudioObject amb;
		AudioObject sfx;

		public:
		AudioFactory();
		void init();
		void free();

	};

}