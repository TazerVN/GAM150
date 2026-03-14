#pragma once

#include "../types.h"
#include "../ECS/ECSystem.h"


namespace Timer
{

	class TimerSystem
	{
		public:
		void update(EntityComponent::Registry&);
		TimerSystem() = default;
	};


}