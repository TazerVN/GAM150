#include "pch.h"
#include "TurnBasedSystem.h"
#include <iomanip>
#include "../system/CardResolver.h"


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
		std::cout << nm << " Turn" << std::endl;
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
		std::cout << "Discard Pile" << std::endl;
		for (Entity e : ecsptr->getComponent<Components::Card_Storage>(playerID)->data_discard_pile)
		{
			std::cout << e;
		}

		std::cout<< std::endl;
		++cur_round;
	}
	void TurnBasedSystem::force_start_if_ready(EntityComponent::Registry& ecs)
	{
		if (!is_active && participants.size() >= 2) start(ecs);
	}

	void TurnBasedSystem::init(EntityComponent::Registry& ecs, EventPool<highlight_tag>& eventPool, Grid::GameBoard& gbp, PhaseSystem::GameBoardState& gbsp,
		CombatNameSpace::CombatSystem& cbs, CardSystem& cs, CardInteraction::CardHand& ch, std::vector<Entity>& entities)
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
			for (size_t i = 0; i < entities.size(); ++i)
			{
				add_participant(ecs, entities[i]);
			}
			force_start_if_ready(ecs);
		}

		gbsptr->GBPTriggered()[static_cast<size_t>(PhaseSystem::GBPhase::START_PHASE)] = true;
	}

	void TurnBasedSystem::add_participant(EntityComponent::Registry& ecs, Entity parti)
	{
		EntityComponent::ComponentTypeID tbs_id = EntityComponent::getComponentTypeID<Components::TurnBasedStats>();

		if (!(ecs.getBitMask()[parti].test(tbs_id)))
		{
			std::cout << "Error entity without TurnBasedStats cannot be added to the turn order!!" << std::endl;
			return;
		}
		participants.push_back(parti);
		participant_hand.push_back(0);	//initialize the card at index 0 as selected by default
		selected_card.push_back(false);

		std::cout << "Added participant : "<< ecs.getComponent<Components::Name>(parti)->value << std::endl;
	}

	void TurnBasedSystem::remove_participant(EntityComponent::Registry& ecs, Entity parti)
	{
		for (int i = 0; i < participants.size(); ++i)
		{
			if (participants[i] == parti)
			{
				participants.erase(participants.begin() + i);	//remove the target from turn system
				participant_hand.erase(participant_hand.begin() + i);
				/*std::cout << "Participant size :" << participants.size() << std::endl;
				std::cout << "Paricipant hand size : " << participant_hand.size() << std::endl;*/
				ecs.destroyEntity(parti);
			}
		}
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
			std::cout << "Can't start no participants" << std::endl;
			return;
		}
		//^I dont think this is necessary - Zejin

		else if (participants.size() < 2)
		{
			std::cout << "Need more than 1 participant to start" << std::endl;
			return;
		}

		is_active = true;
		cur_round = 1;
		cur_player = 0;

		std::cout << "Started Combat" << std::endl;
		std::cout << std::left << std::setw(30) << "Participants" <<  ':' << "Entity ID"<< std::endl;

		for (size_t i = 0; i < participants.size(); ++i)
		{
			std::cout << std::left << std::setw(30) << ecs.getComponent<Components::Name>(participants[i])->value << ":(ID = "
				<< participants[i] << ')' << std::endl;
		}

		cur_round = 1;

		round_start(ecs);  // <-- THIS is the “print Round 1 immediatelyEfix
	}

	//id of active participant
	Entity TurnBasedSystem::current() const
	{
		// If called unsafely, avoid crashing; return 0-ish
		// (If your Entity type is not integer-like, tell me and we’ll adjust.)
		if (!is_active || participants.empty() || cur_player >= participants.size())
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

	void TurnBasedSystem::next(EntityComponent::Registry & ecs)
	{
		if (!is_active) return;

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
		}

		gameBoardptr->unselect_card();
		gameBoardptr->unselect_movement();
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
		gameBoardptr->unselect_card();
		gameBoardptr->unselect_movement();
		int index = get_selected_cardhand_index();

		Entity current_entt = current();
		Entity card = draw_card(ecs, current_entt, index); //draw_card(ecs, current_entt, participant_hand[index]);

		if (card == Components::NULL_INDEX)
		{
			std::cout << "Cannot select invalid index" << std::endl;
			return;
		}

		std::cout << "Attacking with " << ecs.getComponent<Components::Name>(card)->value << std::endl;
		std::cout << "Select Enemy to use card on" << std::endl;
		set_selected_card(true);	//set current participant's selected card to true

		evsptr->template_pool[HIGHLIGHT_EVENT].triggered = true;
		evsptr->template_pool[HIGHLIGHT_EVENT].returned_value = highlight_tag::ATTACK_HIGHLIGHT;

		//if single target do grid select else aoe select
		EntityComponent::ComponentTypeID targCompID = EntityComponent::getComponentTypeID<Components::Targetting_Component>();
		if (!ecs.getBitMask()[card].test(targCompID))
		{
			std::cout << "This card does not have targetting" << std::endl;
			return;
		}

		Components::Targetting_Component* targComp = ecs.getComponent<Components::Targetting_Component>(card);
		
		if(targComp->targetting_type == Targetting::SINGLE_TARGET)
			gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::GRID_SELECT);
		else 
			gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::AOE_GRID_SELECT);
	}

	//return the cardID inside the hand
	Entity TurnBasedSystem::draw_card(EntityComponent::Registry& ecs, Entity player, size_t chIndex)
	{
		Components::Card_Storage* player_storage = ecs.getComponent<Components::Card_Storage>(player);

		return player_storage->data_card_hand[chIndex];
	}

	//returns the status of target being attacked
	PC_RETURN_TAG TurnBasedSystem::play_card(EntityComponent::Registry& ecs, Entity player,Entity target, AEVec2 targetted_pos, int index)
	{
		PC_RETURN_TAG ret = PC_RETURN_TAG::INVALID;

		Entity cardID = this->draw_card(ecs, player, index);
		if (cardID == Components::NULL_INDEX) // Added a guard 
			return PC_RETURN_TAG::INVALID;

		std::string name = ecs.getComponent<Components::Name>(cardID)->value;
		
		/*CardTag* tag = ecs.getComponent<CardTag>(cardID);
		switch (*tag)
		{
		case CardTag::ATTACK:
		{
			cbsptr->play_attack_card(ecs,cardID,target,targetted_pos);
			break;
		}
		default:
			break;
		}

		 ret = PC_RETURN_TAG::VALID;*/

		//remove the card that just played inside tbs
		 f32& card_cost = ecs.getComponent<Components::Card_Cost>(cardID)->value;
		 int& player_curMana = ecs.getComponent<Components::TurnBasedStats>(player)->points;

		 if (card_cost > player_curMana) // Added a not enough mana condition
		 {
			 std::cout << "Not enough mana!!" << std::endl;
			 return PC_RETURN_TAG::INVALID;
		 }

		 ret = CardResolver::resolve(
			 ecs,
			 *cbsptr,
			 *gameBoardptr,
			 *this,
			 player,
			 cardID,
			 target,
			 targetted_pos
		 );

		 // Made it a guard for you Steven - Zejin
		 if (ret == PC_RETURN_TAG::VALID)
		 {
			 player_curMana -= card_cost;
			 remove_card(ecs, player, index);
		 }

		 return ret;
	}

	//DEBUG PRINT
	void TurnBasedSystem::debug_print(EntityComponent::Registry& ecs) const
	{
		//print out info every action
		std::cout << "======================Turn based Stats======================" << std::endl;
		//show deck cards
		std::cout << "Current Actor : " << ecs.getComponent<Components::Name>(current())->value << std::endl;
		show_deck(ecs);
		show_hand(ecs);
		show_discard(ecs);
		show_stats(ecs);
		show_HP(ecs);
		std::cout << "============================================================" << std::endl;
	}

	void TurnBasedSystem::show_HP(EntityComponent::Registry& ecs) const
	{
		for (size_t i = 0; i < participants.size(); ++i)
		{
			/*f32 HP = ecs.getComponent<Components::HP>(participants[i])->c_value;
			char const* name = ecs.getComponent<Components::Name>(participants[i])->value;
			std::cout << name << "'s HP : " << HP << " | " << std::endl;*/
			EntityComponent::ComponentTypeID hpID = EntityComponent::getComponentTypeID<Components::HP>();

			std::string name = ecs.getComponent<Components::Name>(participants[i])->value;

			if (ecs.getBitMask()[participants[i]].test(hpID))
			{
				f32 HP = ecs.getComponent<Components::HP>(participants[i])->c_value;
				std::cout << name << "'s HP : " << HP << " | " << std::endl;
			}
			else
			{
				std::cout << name << " (no HP)" << std::endl;
			}
		}
	}

	void TurnBasedSystem::show_deck(EntityComponent::Registry& ecs) const
	{
		std::cout << "---------Player's Deck---------" << std::endl;

		Components::Card_Storage* playerStorage = ecs.getComponent<Components::Card_Storage>(playerID);
		size_t sz = playerStorage->data_deck.size();
		for (size_t i = 0; i < sz; ++i)
		{
			Entity cardID = playerStorage->data_deck[i];
			Components::Name* name = ecs.getComponent<Components::Name>(cardID);
			std::cout << ((cardID == -1) ? "NULL INDEX" : name->value) << ((i == sz - 1) ? "\n" : " | ");
		}
		std::cout << "-------------------------------" << std::endl;
	}

	void TurnBasedSystem::show_hand(EntityComponent::Registry& ecs) const
	{
		//disolay player hands
		std::cout << "---------Player's Hand---------" << std::endl;
		Components::Card_Storage* playerStorage = ecs.getComponent<Components::Card_Storage>(playerID);
		size_t sz = playerStorage->data_card_hand.size();
		for (size_t i = 0; i < sz; ++i)
		{
			Entity cardID = playerStorage->data_card_hand[i];
			Components::Name* name = ecs.getComponent<Components::Name>(cardID);
			std::cout << ((cardID == -1)? "NULL INDEX" : name->value) << ((i == sz - 1) ? "\n" : " | ");
		}
		std::cout << "------------------------------" << std::endl;
	}

	void TurnBasedSystem::show_discard(EntityComponent::Registry& ecs) const
	{
		//disolay player hands
		std::cout << "---------Player's Discard Piler---------" << std::endl;
		Components::Card_Storage* playerStorage = ecs.getComponent<Components::Card_Storage>(playerID);
		size_t sz = playerStorage->data_discard_pile.size();
		for (size_t i = 0; i < sz; ++i)
		{
			Entity cardID = playerStorage->data_discard_pile[i];
			Components::Name* name = ecs.getComponent<Components::Name>(cardID);
			std::cout << ((cardID == -1) ? "NULL INDEX" : name->value) << ((i == sz - 1) ? "\n" : " | ");
		}
		std::cout << "----------------------------------------" << std::endl;
	}

	void TurnBasedSystem::show_stats(EntityComponent::Registry& ecs) const
	{
		std::cout << "---------Player's Stats---------" << std::endl;
		Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(this->current());
		std::cout << "Points : " << stats->points << '/' << stats->maxPoints << std::endl;
		std::cout << "SPD : " << stats->cur_movSpd << std::endl;
		std::cout << "Shields : " << stats->shields << std::endl;
		std::cout << "--------------------------------" << std::endl;
	}

	void TurnBasedSystem::DrawPhase_add_card(EntityComponent::Registry& ecs)
	{
		//get random card from the player's deck
		Components::Card_Storage* storage = ecs.getComponent<Components::Card_Storage>(playerID);
		std::vector<size_t>& deck = storage->data_deck;

		if (deck.empty())
		{
			storage->reshuffle_discard2deck();

			if (deck.empty())
			{
				std::cout << "Error" << std::endl;
				return;
			}
		}

		int index = int(AERandFloat() * deck.size());
		Entity card = deck[index];
		EntityFactory::add_card_player_hand(ecs, playerID, card);	//add a random card
		//remove the card afterwards
		deck.erase(deck.begin() + index);
	}

	void TurnBasedSystem::remove_card(EntityComponent::Registry& ecs,Entity user,int index)
	{
		EntityFactory::remove_card_player(ecs, user, index);	//this is to remove data from ecs
		cardHandptr->remove_card(ecs,index);		//this is for visual side
	}

	bool TurnBasedSystem::update() const
	{
		return (participants.size() == 1 && participants[0] == playerID);
	}
	void TurnBasedSystem::tbs_free()
	{
		cur_player = -1;
		is_active = false;
		cur_round = 0;

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
