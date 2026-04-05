//=========================================
//	AUTHOR:		PHAM MINH TUAN
// 
//	EMAIL:		minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//=========================================
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
		void init();
		void update(Grid::GameBoard& gb, PhaseSystem::GameBoardState& gbs,CombatNameSpace::CombatSystem& cs);
		void free();
	};

	void init_animation_for_entity(Entity ent);
};
