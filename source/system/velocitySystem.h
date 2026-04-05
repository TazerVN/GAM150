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

