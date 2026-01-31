#pragma once
#include <vector>
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../cards/Systems.h"
#include "AEEngine.h"
namespace TBS 
{
	class TurnBasedSystem
	{
	private:
		bool is_active{ false };
		size_t cur_round{0};
		size_t index{0};
		std::vector<Entity> participants;
	public:
		//===========Set Ups============================
		void add_participant(ECS::Registry& ecs,Entity parti);
		void start(ECS::Registry& ecs);
		Entity current();
		void next();
		void end();
		bool active();
		size_t round();

		//============Combat=======================
		Entity draw_card(ECS::Registry& ecs, Entity player, size_t chIndex);

		//===============Update=====================
	};
}