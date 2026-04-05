//=========================================
//	AUTHOR:		PHAM MINH TUAN
// 
//	EMAIL:		minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//=========================================
#include "pch.h"
#include "TimerSystem.h"

namespace Timer{

void TimerSystem::update()
{
	EntityComponent::ComponentTypeID timerID = EntityComponent::getComponentTypeID<Components::Timer>();

	EntityComponent::ComponentBitMask objMask;
	objMask.set(timerID);


	for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
	{
		if ((it->first & objMask) == objMask)
		{
			f32 dt = f32(AEFrameRateControllerGetFrameTime());
			for (Entity ent : it->second)
			{
				Components::Timer* timer = ecs.getComponent<Components::Timer>(ent);
				if(timer->start == false) timer->seconds = 0;
				if(timer->reset == true && timer->seconds >= timer->max_seconds){
					timer->seconds = 0;
				}
				else if(timer->start == true && timer->seconds < timer->max_seconds)
				{
					if(!::pause) timer->seconds += dt;
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