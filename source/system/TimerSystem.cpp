#include "TimerSystem.h"
#include "../ECS/ECSystem.h"

namespace TimerSystem{

void TimerSystem::update(ECS::Registry& ecs)
{
	ECS::ComponentTypeID timerID = ECS::getComponentTypeID<Components::Timer>();

	ECS::ComponentBitMask objMask;
	objMask.set(timerID);


	for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
	{
		if ((it->first & objMask) == objMask)
		{
			f32 dt = f32(AEFrameRateControllerGetFrameTime());
			for (Entity ent : it->second)
			{
				Components::Timer* timer = ecs.getComponent<Components::Timer>(ent);
				if(timer->reset == true){
					timer->start = true;
					timer->reset = false;
					timer->seconds = 0;
				}
				else if(timer->start == true && timer->seconds < timer->max_seconds)
				{
					timer->seconds += dt;
				}
				else if(timer->seconds >= timer->max_seconds)
				{
					timer->start = false;
				}
			}
		}
	}
	
}
}