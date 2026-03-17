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

		void round_start(EntityComponent::Registry& ecs);
		void round_end();
	public:
		//===========Set Ups============================
		void init(EntityComponent::Registry&, EventPool<highlight_tag>&, Grid::GameBoard&, PhaseSystem::GameBoardState&, CombatNameSpace::CombatSystem&, CardSystem&, CardInteraction::CardHand& ,std::vector<Entity>&);
		void add_participant(EntityComponent::Registry& ecs,Entity parti);
		void remove_participant(EntityComponent::Registry& ecs, Entity parti);
		std::vector<Entity>& get_participant();

		//returns ID of the active participant in turn order
		Entity current() const;
		//return the current index of cardhand the active participant is holding
		int get_selected_cardhand_index() const;
		//determine if the active participant has selected a card or not
		bool is_current_selected_card() const;
		void set_selected_card(bool bol);
		void next(EntityComponent::Registry& ecs);	
		void end();

		bool& active();
		bool active() const;
		std::vector<int>& hand();
		size_t round();
		//============Turn Control=============
		void start(EntityComponent::Registry& ecs);
		void force_start_if_ready(EntityComponent::Registry& ecs);  // starts automatically when participants >=2
		void debug_print(EntityComponent::Registry& ecs) const;

		//==============Player Stats==============
		void show_deck(EntityComponent::Registry& ecs) const;
		void show_HP(EntityComponent::Registry& ecs) const;
		void show_hand(EntityComponent::Registry& ecs) const;
		void show_discard(EntityComponent::Registry& ecs) const;
		void show_stats(EntityComponent::Registry& ecs) const;


		void next_enemy_or_player(EntityComponent::Registry& ecs);	// For Enemy CPU
		void yield_to_player(EntityComponent::Registry& ecs);

		//============Combat=======================

		PC_RETURN_TAG play_card(EntityComponent::Registry& ecs, Entity player, Entity target, AEVec2 targetted_pos, int index);

		void DrawPhase_add_card(EntityComponent::Registry& ecs);
		void remove_card(EntityComponent::Registry& ecs, Entity user, int index);
		void select_hand_index(size_t index);
		//by pressing U player select his card
		void select_card(EntityComponent::Registry& ecs);
		//draw the card of the player
		Entity draw_card(EntityComponent::Registry& ecs, Entity player, size_t chIndex);

		bool update() const;
		void tbs_free();
	};
}