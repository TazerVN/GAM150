
#pragma once

#include "../types.h"
#include "../ECS/Components.h"
#include "../ECS/ECSystem.h"
#include "../system/CombatSystem.h"

namespace Animation
{
	class AnimationSystem
	{

		public:
		AnimationSystem();
		void init(EntityComponent::Registry& ecs);
		void update(EntityComponent::Registry& ecs, Grid::GameBoard& gb, PhaseSystem::GameBoardState& gbs,CombatNameSpace::CombatSystem& cs);
	};
};
