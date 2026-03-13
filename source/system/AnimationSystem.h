
#pragma once

#ifndef ANIMATION
#define ANIMATION


#include "../ECS/Components.h"
#include "../ECS/ECSystem.h"

namespace Animation
{
	class AnimationSystem
	{

		public:
		AnimationSystem();
		void init(ECS::Registry& ecs);
		void update(ECS::Registry& ecs);
	};
};
#endif