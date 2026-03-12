#ifndef VelocitySystem_H
#define VelocitySystem_H

#include "AEEngine.h"
#include "../ECS/ECSystem.h"

namespace VelocitySystem
{
	class VelocitySystem
	{
	public:
		void update(ECS::Registry& ecs);
		VelocitySystem() = default;
	};

}

#endif //VelocitySystem_H