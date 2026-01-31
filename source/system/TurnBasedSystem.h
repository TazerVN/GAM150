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

		// Actors on Borad
		size_t index{0};
		std::vector<Entity> participants;

		enum class GM : uint8_t { Player = 0, Enemy = 1 };

		// Game Master Turn and Yield States
		GM current_gm{ GM::Player };
		bool gm_yielded[2]{ false, false };

		// First GM to yield gets first turn next round
		bool first_yield_set{ false };
		GM first_yielder{ GM::Player };
		size_t gm_turn_count{ 0 };

		// Helpers
		static constexpr int gm_index(GM gm) { return (gm == GM::Player) ? 0 : 1; }
		static const char* gm_name(GM gm) { return (gm == GM::Player) ? "Player" : "Enemy"; }

		bool everyone_yielded() const;
		void round_start(ECS::Registry& ecs);
	public:
		//===========Set Ups============================
		void add_participant(ECS::Registry& ecs,Entity parti);
		void start(ECS::Registry& ecs);

		Entity current();
		void next(ECS::Registry& ecs);		// advance turn after successful action OR yield
		void end();

		bool active();
		size_t round();

		//============Yield / Turn Control=============
		void yield_current();				// current participant yields
		void force_start_if_ready(ECS::Registry& ecs);  // starts automatically when participants >=2
		void debug_print(ECS::Registry& ecs) const;

		//============Combat=======================
		Entity draw_card(ECS::Registry& ecs, Entity player, size_t chIndex);

		//===============Update=====================
	};
}