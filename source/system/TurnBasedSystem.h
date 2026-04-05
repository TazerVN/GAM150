#pragma once

#include "../types.h"

#include "../ECS/ECSystem.h"
#include "../system/CardSystem.h"
#include "../ECS/Components.h"
#include "../util/Event.h"
//#include "../system/PhaseSystem.h"
//#include "../UI/cardInteraction.h"
#include <vector>
#include <iostream>
#include "AEEngine.h"

//forward declaration so that turnbased system have access to gridsystem
namespace Grid { class GameBoard; };
namespace CardInteraction { class CardHand; };
namespace CombatNameSpace { class CombatSystem; };
namespace PhaseSystem { class GameBoardState; }

namespace TBS 
{
	class TurnBasedSystem
	{
	private:
		size_t cur_player{ 0 };
		bool is_active{ false };
		size_t cur_round{0};

		EntityComponent::Registry* ecsptr = nullptr;
		EventPool<highlight_tag>* evsptr = nullptr;
		Grid::GameBoard* gameBoardptr = nullptr;
		PhaseSystem::GameBoardState* gbsptr = nullptr;
		CardSystem* cardSysptr = nullptr;
		CardInteraction::CardHand* cardHandptr = nullptr;
		CombatNameSpace::CombatSystem* cbsptr = nullptr;

		// Actors on Borad
		std::vector<Entity> participants;
		//hand index not the entity of the card
		std::vector<int> participant_hand;	
		//has the current participant selected a card
		std::vector<bool> selected_card;

		void round_start();
		void round_end();
	public:
		//===========Set Ups============================
		void init(EventPool<highlight_tag>&, Grid::GameBoard&, PhaseSystem::GameBoardState&, CombatNameSpace::CombatSystem&
			, CardSystem&, CardInteraction::CardHand& ,Entity);
		void add_participant(Entity parti);
		std::vector<Entity>& get_participant();

		//returns ID of the active participant in turn order
		Entity current() const;
		//return the current index of cardhand the active participant is holding
		int get_selected_cardhand_index() const;
		//determine if the active participant has selected a card or not
		bool is_current_selected_card() const;
		void set_selected_card(bool bol);
		void next();	
		void end();

		bool& active();
		bool active() const;
		std::vector<int>& hand();
		size_t round();
		//============Turn Control=============
		void start();
		void force_start_if_ready();  // starts automatically when participants >=2
		void debug_print() const;

		//==============Player Stats==============
		/*void show_deck(EntityComponent::Registry& ecs) const;
		void show_HP(EntityComponent::Registry& ecs) const;
		void show_hand(EntityComponent::Registry& ecs) const;
		void show_discard(EntityComponent::Registry& ecs) const;
		void show_stats(EntityComponent::Registry& ecs) const;*/

		//============Combat=======================
	
		void DrawPhase_add_card();
		void select_hand_index(int index);
		//by pressing U player select his card
		void select_card();

		bool update();
		void tbs_free();
	};
}