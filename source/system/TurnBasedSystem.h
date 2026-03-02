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
namespace CardInteraction { class CardHand; };

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
		CardInteraction::CardHand* cardHandptr = nullptr;


		// Actors on Borad
		std::vector<Entity> participants;
		//hand index not the entity of the card
		std::vector<size_t> participant_hand;	
		//has the current participant selected a card
		std::vector<bool> selected_card;

		void round_start(ECS::Registry& ecs);
		void round_end();
	public:
		//===========Set Ups============================
		void init(ECS::Registry&, EventPool&, Grid::GameBoard&, PhaseSystem::GameBoardState&, System::CardSystem&, CardInteraction::CardHand& ,std::vector<Entity>&);
		void add_participant(ECS::Registry& ecs,Entity parti);
		void remove_participant(ECS::Registry& ecs, Entity parti);

		//returns ID of the active participant in turn order
		Entity current();
		//return the current index of cardhand the active participant is holding
		Entity get_selected_cardhand_index();
		//determine if the active participant has selected a card or not
		bool is_current_selected_card();
		void set_selected_card(bool bol);
		void next(ECS::Registry& ecs);	
		void end();

		bool active();
		std::vector<size_t>& hand();
		size_t round();
		//============Turn Control=============
		void start(ECS::Registry& ecs);
		void force_start_if_ready(ECS::Registry& ecs);  // starts automatically when participants >=2
		void debug_print(ECS::Registry& ecs) const;
		void show_HP(ECS::Registry& ecs) const;
		void show_hand(ECS::Registry& ecs) const;

		//============Combat=======================
		void check_input(ECS::Registry&);
		void add_card(ECS::Registry& ecs);
		void select_hand_index(size_t index);
		//by pressing U player select his card
		void select_card(ECS::Registry& ecs);
		//draw the card of the player
		Entity draw_card(ECS::Registry& ecs, Entity player, size_t chIndex);
		//function to determine the play card it could be attack it could be defense or support
		bool play_card(ECS::Registry& ecs,Entity target, Entity cardID);
		bool Call_AttackSystem(ECS::Registry& ecs, Entity cardID, Entity target);
		void Call_DefenseSystem(ECS::Registry& ecs, Entity cardID, Entity target);
		//===============Update=====================
		void update(ECS::Registry& ecs);

		void update_GBPhasetriggered();
		void update_GBPhaseUpdate(ECS::Registry&);
	};
}