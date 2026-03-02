#pragma once
#include "../ECS/ECSystem.h"

namespace TimerSystem
{

	class TimerSystem
	{
		public:
		void update(ECS::Registry&);
		TimerSystem() = default;
	};


}