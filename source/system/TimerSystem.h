//=========================================
//	AUTHOR:		PHAM MINH TUAN
// 
//	EMAIL:		minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//=========================================
#pragma once

#include "../types.h"
#include "../ECS/ECSystem.h"


namespace Timer
{

	class TimerSystem
	{
		public:
		void update();
		TimerSystem() = default;
	};


}