//===============================================================
//	AUTHOR:		Tio Chun Yi
// 
//	EMAIL:		tio.c@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once

#include "../types.h"
#include "AEEngine.h"
#include "../ECS/ECSystem.h"

namespace Velocity
{
	class VelocitySystem
	{
	public:
		void update();
		VelocitySystem() = default;
	};

}

