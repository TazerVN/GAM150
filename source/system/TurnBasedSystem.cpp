#include "pch.h"
#include "TurnBasedSystem.h"
#include <iomanip>
#include "../system/CardResolver.h"
#include "../system/PhaseSystem.h"
#include "../UI/cardInteraction.h"
#include "../level/game.h"
#include <random>



namespace TBS
{
	// round start helper forcing it to start yadda yadda
	void TurnBasedSystem::round_start(EntityComponent::Registry& ecs)
	{
		cur_player = 0;
		gbsptr->resetGPhase();
		gbsptr->GBPTriggered()[static_cast<size_t>(gbsptr->getGBPhase())] = true;
		gbsptr->resetPlayerPhase();

		std::cout << "\n=== ROUND " << cur_round << " START ===\n";
		std::string& nm = ecs.getComponent<Components::Name>(participants[cur_player])->value;
		std::cout << nm << " Turn" << '\n';
	}
	void TurnBasedSystem::round_end()
	{
		std::vector<Entity>& vec = ecsptr->getComponent<Components::Card_Storage>(playerID)->data_card_hand;
		size_t sz = vec.size();
		while (!vec.empty())
		{
			EntityFactory::remove_card_player(*ecsptr, playerID, 0);	//this is to remove data from ecs
			cardHandptr->remove_card(*ecsptr, 0);		//this is for visual side
		}

		//show discard pile
		std::cout << "Discard Pile" << '\n';
		for (Entity e : ecsptr->getComponent<Components::Card_Storage>(playerID)->data_discard_pile)
		{
			std::cout << e;
		}

		std::cout<< '\n';
		++cur_round;
	}
	void TurnBasedSystem::force_start_if_ready(EntityComponent::Registry& ecs)
	{
		if (!is_active && participants.size() >= 2) start(ecs);
	}

	void TurnBasedSystem::init(EventPool<highlight_tag>& eventPool, Grid::GameBoard& gbp, PhaseSystem::GameBoardState& gbsp,
		CombatNameSpace::CombatSystem& cbs, CardSystem& cs, CardInteraction::CardHand& ch, Entity horde)
	{
		evsptr = &eventPool;
		//highlight event
		evsptr->template_pool.push_back(EventTemplate<highlight_tag>{});
		//unhighlight event
		evsptr->template_pool.push_back(EventTemplate<highlight_tag>{});

		ecsptr = &ecs;
		gbsptr = &gbsp;
		cardSysptr = &cs;
		gameBoardptr = &gbp;
		cardHandptr = &ch;
		cbsptr = &cbs;
	
		if (!is_active)
		{
			add_participant(ecs, playerID);
			add_participant(ecs, horde);
			force_start_if_ready(ecs);
		}

		gbsptr->GBPTriggered()[static_cast<size_t>(PhaseSystem::GBPhase::START_PHASE)] = true;
	}

	void TurnBasedSystem::add_participant(EntityComponent::Registry& ecs, Entity parti)
	{
		EntityComponent::ComponentTypeID tbs_id = EntityComponent::getComponentTypeID<Components::TurnBasedStats>();

		if (!(ecs.getBitMask()[parti].test(tbs_id)))
		{
			std::cout << "Error entity without TurnBasedStats cannot be added to the turn order!!" << '\n';
			return;
		}
		participants.push_back(parti);
		participant_hand.push_back(0);	//initialize the card at index 0 as selected by default
		selected_card.push_back(false);

		std::cout << "Added participant : "<< ecs.getComponent<Components::Name>(parti)->value << '\n';
	}

	void TurnBasedSystem::remove_participant(EntityComponent::Registry& ecs, Entity parti)
	{
		//for (int i = 0; i < participants.size(); ++i)
		//{
		//	if (participants[i] == parti)
		//	{
		//		participants.erase(participants.begin() + i);	//remove the target from turn system
		//		participant_hand.erase(participant_hand.begin() + i);
		//		selected_card.erase(selected_card.begin() + i);
		//		ecs.destroyEntity(parti);
		//	}
		//}
	}

	std::vector<Entity>& TurnBasedSystem::get_participant()
	{
		return participants;
	}

	void TurnBasedSystem::start(EntityComponent::Registry& ecs)
	{
		//check for edge cases
		if (participants.empty())
		{
			std::cout << "Can't start no participants" << '\n';
			return;
		}
		//^I dont think this is necessary - Zejin

		else if (participants.size() < 2)
		{
			std::cout << "Need more than 1 participant to start" << '\n';
			return;
		}

		is_active = true;
		cur_round = 1;
		cur_player = 0;

		std::cout << "Started Combat" << '\n';
		std::cout << std::left << std::setw(30) << "Participants" <<  ':' << "Entity ID"<< '\n';

		for (size_t i = 0; i < participants.size(); ++i)
		{
			std::cout << std::left << std::setw(30) << ecs.getComponent<Components::Name>(participants[i])->value << ":(ID = "
				<< participants[i] << ')' << '\n';
		}

		cur_round = 1;

		round_start(ecs);  // <-- THIS is the “print Round 1 immediatelyEfix
	}

	//id of active participant
	Entity TurnBasedSystem::current() const
	{
		// If called unsafely, avoid crashing; return 0-ish
		// (If your Entity type is not integer-like, tell me and we’ll adjust.)
		if (participants.empty() || cur_player >= participants.size())
			return static_cast<Entity>(-1);

		return participants[cur_player];
	}

	//returns the index of card hand the player has selected
	int TurnBasedSystem::get_selected_cardhand_index() const
	{
		return participant_hand[cur_player];
	}

	bool TurnBasedSystem::is_current_selected_card() const
	{
		return selected_card[cur_player];
	}

	void TurnBasedSystem::set_selected_card(bool bol)
	{
		selected_card[cur_player] = bol;
	}

	void TurnBasedSystem::next()
	{
		if (!is_active) return;
		if(gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::PLAYER_ANIMATION) return;
		++cur_player;
		if (cur_player >= participants.size())
		{
			round_end();
			round_start(ecs);
			return;
		}

		if (current() == playerID)
		{
			gbsptr->resetPlayerPhase();
			//gbsptr->set_GBPhase(PhaseSystem::GBPhase::MAIN_PHASE);
		}
		
		//gameBoardptr->unselect_card();
		//gameBoardptr->unselect_movement();
		debug_print(ecs);
	}

	void TurnBasedSystem::end()
	{
		is_active = false;
		cur_round = 0;
		cur_player = 0;
		participants.clear();
		participant_hand.clear();
		selected_card.clear();
		std::cout << "[DBG] TBS end() CALLED\n";
	}

	bool& TurnBasedSystem::active()
	{
		return is_active;
	}

	bool TurnBasedSystem::active() const
	{
		return is_active;
	}

	std::vector<int>& TurnBasedSystem::hand()
	{
		return participant_hand;
	}

	size_t TurnBasedSystem::round()
	{
		return cur_round;
	}

	void TurnBasedSystem::select_hand_index(size_t index)
	{
		participant_hand[cur_player] = index;
	}

	void TurnBasedSystem::select_card(EntityComponent::Registry& ecs)
	{
		if (this->is_current_selected_card())
			gameBoardptr->unselect_card();
		if (gameBoardptr->selected_player())
			gameBoardptr->unselect_movement();

		//gameBoardptr->unselect_movement();
		int index = get_selected_cardhand_index();

		Entity current_entt = current();
		Entity card = cbsptr->draw_card(current_entt, index); //draw_card(ecs, current_entt, participant_hand[index]);

		if (card == Components::NULL_INDEX)
		{
			std::cout << "Cannot select invalid index" << '\n';
			return;
		}

		std::cout << "Select Enemy to use card on" << '\n';
		set_selected_card(true);	//set current participant's selected card to true

		evsptr->template_pool[HIGHLIGHT_EVENT].triggered = true;
		evsptr->template_pool[HIGHLIGHT_EVENT].returned_value = highlight_tag::ATTACK_HIGHLIGHT;

		//if single target do grid select else aoe select
		EntityComponent::ComponentTypeID targCompID = EntityComponent::getComponentTypeID<Components::Targetting_Component>();
		if (!ecs.getBitMask()[card].test(targCompID))
		{
			std::cout << "This card does not have targetting" << '\n';
			return;
		}

		Components::Targetting_Component* targComp = ecs.getComponent<Components::Targetting_Component>(card);
		
		if(targComp->targetting_type == Targetting::AOE)
			gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::AOE_GRID_SELECT);
		else 
			gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::GRID_SELECT);
	}
	//DEBUG PRINT
	void TurnBasedSystem::debug_print(EntityComponent::Registry& ecs) const
	{
		//print out info every action
		std::cout << "======================Turn based Stats======================" << '\n';
		//show deck cards
		std::cout << "Current Actor : " << ecs.getComponent<Components::Name>(current())->value << '\n';
		show_deck(ecs);
		show_hand(ecs);
		show_discard(ecs);
		show_stats(ecs);
		show_HP(ecs);
		std::cout << "============================================================" << '\n';
	}

	void TurnBasedSystem::show_HP(EntityComponent::Registry& ecs) const
	{
		for (size_t i = 0; i < participants.size(); ++i)
		{
			/*f32 HP = ecs.getComponent<Components::HP>(participants[i])->c_value;
			char const* name = ecs.getComponent<Components::Name>(participants[i])->value;
			std::cout << name << "'s HP : " << HP << " | " << '\n';*/
			EntityComponent::ComponentTypeID hpID = EntityComponent::getComponentTypeID<Components::HP>();
			EntityComponent::ComponentTypeID hordeTagID = EntityComponent::getComponentTypeID<Components::Horde_Tag>();

			std::string name = ecs.getComponent<Components::Name>(participants[i])->value;

			if (ecs.getBitMask()[participants[i]].test(hpID))
			{
				f32 HP = ecs.getComponent<Components::HP>(participants[i])->c_value;
				std::cout << name << "'s HP : " << HP << " | " << '\n';
			}
			else
			{
				if (ecs.getBitMask()[participants[i]].test(hordeTagID))
				{
					std::vector<Entity> goons = ecs.getComponent<Components::Horde_Tag>(participants[i])->goons;
					for (Entity goon : goons)
					{
						std::string gname = ecs.getComponent<Components::Name>(goon)->value;
						f32 HP = ecs.getComponent<Components::HP>(goon)->c_value;
						std::cout << gname << "'s HP : " << HP << " | " << '\n';
					}
				}
				else
				{
					std::cout << name << " (no HP)" << '\n';
				}
			}
		}
	}

	void TurnBasedSystem::show_deck(EntityComponent::Registry& ecs) const
	{
		std::cout << "---------Player's Deck---------" << '\n';

		Components::Card_Storage* playerStorage = ecs.getComponent<Components::Card_Storage>(playerID);
		size_t sz = playerStorage->data_draw_pile.size();
		for (size_t i = 0; i < sz; ++i)
		{
			Entity cardID = playerStorage->data_draw_pile[i];
			Components::Name* name = ecs.getComponent<Components::Name>(cardID);
			std::cout << ((cardID == -1) ? "NULL INDEX" : name->value) << ((i == sz - 1) ? "\n" : " | ");
		}
		std::cout << "-------------------------------" << '\n';
	}

	void TurnBasedSystem::show_hand(EntityComponent::Registry& ecs) const
	{
		//disolay player hands
		std::cout << "---------Player's Hand---------" << '\n';
		Components::Card_Storage* playerStorage = ecs.getComponent<Components::Card_Storage>(playerID);
		size_t sz = playerStorage->data_card_hand.size();
		for (size_t i = 0; i < sz; ++i)
		{
			Entity cardID = playerStorage->data_card_hand[i];
			Components::Name* name = ecs.getComponent<Components::Name>(cardID);
			std::cout << ((cardID == -1)? "NULL INDEX" : name->value) << ((i == sz - 1) ? "\n" : " | ");
		}
		std::cout << "------------------------------" << '\n';
	}

	void TurnBasedSystem::show_discard(EntityComponent::Registry& ecs) const
	{
		//disolay player hands
		std::cout << "---------Player's Discard Piler---------" << '\n';
		Components::Card_Storage* playerStorage = ecs.getComponent<Components::Card_Storage>(playerID);
		size_t sz = playerStorage->data_discard_pile.size();
		for (size_t i = 0; i < sz; ++i)
		{
			Entity cardID = playerStorage->data_discard_pile[i];
			Components::Name* name = ecs.getComponent<Components::Name>(cardID);
			std::cout << ((cardID == -1) ? "NULL INDEX" : name->value) << ((i == sz - 1) ? "\n" : " | ");
		}
		std::cout << "----------------------------------------" << '\n';
	}

	void TurnBasedSystem::show_stats(EntityComponent::Registry& ecs) const
	{
		std::cout << "---------Player's Stats---------" << '\n';
		Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(this->current());
		std::cout << "Points : " << stats->points << '/' << stats->maxPoints << '\n';
		std::cout << "SPD : " << stats->cur_movSpd << '\n';
		std::cout << "Shields : " << stats->shields << '\n';
		std::cout << "--------------------------------" << '\n';
	}

	void TurnBasedSystem::DrawPhase_add_card(EntityComponent::Registry& ecs)
	{
		//get random card from the player's deck
		Components::Card_Storage* storage = ecs.getComponent<Components::Card_Storage>(playerID);
		std::vector<size_t>& drawPile = storage->data_draw_pile;

		if (drawPile.empty())
		{
			storage->reshuffle_discard2deck();

			if (drawPile.empty())
			{
				std::cout << "Error" << '\n';
				return;
			}
		}

		int upper_bound = drawPile.size() - 1; int lower_bound = 0;
		int index = std::rand() % (upper_bound - lower_bound + 1) + lower_bound;
		//int index = int(AERandFloat() * drawPile.size());	<- old rand code
		Entity card = drawPile[index];
		EntityFactory::add_card_player_hand(ecs, playerID, card);	//add a random card
		//remove the card afterwards
		drawPile.erase(drawPile.begin() + index);
	}

	bool TurnBasedSystem::update()
	{
		if (!player_died)
		{
			float cur_Hp = ecs.getComponent<Components::HP>(playerID)->c_value;
			if (cur_Hp <= 0.f)
			{
				player_died = true;
				//set player's win speed
				Components::TurnBasedStats* player = ecs.getComponent < Components::TurnBasedStats>(playerID);
				player->cur_movSpd = player->ini_movSpd;
				ecs.getComponent<Components::Card_Storage>(playerID)->free();
				ecs.getComponent<Components::Card_Storage>(playerID)->reset();
			}
		}
		return !(ecs.getComponent<Components::Horde_Tag>(participants[1])->alive());
	}
	void TurnBasedSystem::tbs_free()
	{
		cur_player = -1;
		is_active = false;
		cur_round = 0;
		player_died = false;
		ecsptr = nullptr;
		evsptr = nullptr;
		gameBoardptr = nullptr;
		gbsptr = nullptr;
		cardSysptr = nullptr;
		cardHandptr = nullptr;
		cbsptr = nullptr;
		
		participants.clear();
		participant_hand.clear();
		selected_card.clear();
	}
}
