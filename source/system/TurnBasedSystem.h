#pragma once
#include <vector>
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../factory/EntityFactory.h"
#include "../util/Event.h"
#include "../system/PhaseSystem.h"
#include "AEEngine.h"

//forward declaration so that turnbased system have access to gridsystem
namespace Grid { class GameBoard; };

namespace TBS 
{
	class TurnBasedSystem
	{
	private:
		size_t cur_player{ 0 };
		bool is_active{ false };
		size_t cur_round{0};

		EventPool* evsptr = nullptr;
		Grid::GameBoard* gameBoardptr = nullptr;
		PhaseSystem::GameBoardState* gbsptr = nullptr;
		System::CardSystem* cardSysptr = nullptr;

		// Actors on Borad
		std::vector<Entity> participants;
		std::vector<size_t> participant_hand;	//hand index not the entity of the card
		std::vector<bool> selected_card;
		enum class GM : uint8_t { Player = 0, Enemy = 1 };

		void round_start(ECS::Registry& ecs);
		void round_end();
	public:
		//===========Set Ups============================
		void init(EventPool&, Grid::GameBoard&, PhaseSystem::GameBoardState&, System::CardSystem&);
		void add_participant(ECS::Registry& ecs,Entity parti);
		void remove_participant(ECS::Registry& ecs, Entity parti);
		void start(ECS::Registry& ecs);

		Entity current();
		Entity get_selected_cardhand_index();
		bool is_current_selected_card();
		void set_selected_card(bool bol);
		void next(ECS::Registry& ecs);	
		void end();

		bool active();
		std::vector<size_t>& hand();
		size_t round();
		//============Yield / Turn Control=============
		void force_start_if_ready(ECS::Registry& ecs);  // starts automatically when participants >=2
		void debug_print(ECS::Registry& ecs) const;
		void show_HP(ECS::Registry& ecs) const;
		void show_hand(ECS::Registry& ecs) const;

		//============Combat=======================
		void add_card(ECS::Registry& ecs);
		void select_card(size_t index);
		Entity draw_card(ECS::Registry& ecs, Entity player, size_t chIndex);
		bool play_card(ECS::Registry& ecs,Entity target, Entity cardID);
		bool Call_AttackSystem(ECS::Registry& ecs, Entity cardID, Entity target);
		//===============Update=====================
		void update(ECS::Registry& ecs,std::vector<Entity>& entities);

		void update_GBPhasetriggered();
		void update_GBPhaseUpdate(ECS::Registry&);
	};
}