
#pragma once

#include "../types.h"
#include "../ECS/Components.h"
#include "../ECS/ECSystem.h"



namespace Animation
{
	class AnimationSystem
	{

		public:
		AnimationSystem();
		void init(EntityComponent::Registry& ecs);
		void update(EntityComponent::Registry& ecs, Grid::GameBoard& gb, CombatNameSpace::CombatSystem& cs);
	};
};
