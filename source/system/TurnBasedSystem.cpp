#include "TurnBasedSystem.h"
#include "../system/GridSystem.h"
#include "../util/util.h"
#include "../UI/cardInteraction.h"
#include "../global.h"
#include "CombatSystem.h"
#include <iomanip>
#include "../factory/EntityFactory.h"
namespace TBS
{
	// round start helper forcing it to start yadda yadda
	void TurnBasedSystem::round_start(ECS::Registry& ecs)
	{
		cur_player = 0;
		gbsptr->resetGPhase();
		gbsptr->GBPTriggered()[static_cast<size_t>(gbsptr->getGBPhase())] = true;
		gbsptr->resetPlayerPhase();

		std::cout << "\n=== ROUND " << cur_round << " START ===\n";
		char const* nm = ecs.getComponent<Components::Name>(participants[cur_player])->value;
		std::cout << nm << " Turn" << std::endl;
	}
	void TurnBasedSystem::round_end()
	{
		std::vector<Entity>& vec = ecsptr->getComponent<Components::Card_Storage>(playerID)->data_card_hand;
		size_t sz = vec.size();
		while (!vec.empty())
		{
			System::remove_card_player(*ecsptr, playerID, 0);	//this is to remove data from ecs
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
	void TurnBasedSystem::force_start_if_ready(ECS::Registry& ecs)
	{
		if (!is_active && participants.size() >= 2) start(ecs);
	}

	void TurnBasedSystem::init(ECS::Registry& ecs, EventPool<highlight_tag>& eventPool, Grid::GameBoard& gbp, PhaseSystem::GameBoardState& gbsp,
		CardSystem& cs, CardInteraction::CardHand& ch, std::vector<Entity>& entities)
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

	void TurnBasedSystem::add_participant(ECS::Registry& ecs, Entity parti)
	{
		ECS::ComponentTypeID tbs_id = ECS::getComponentTypeID<Components::TurnBasedStats>();

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

	void TurnBasedSystem::remove_participant(ECS::Registry& ecs, Entity parti)
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

	void TurnBasedSystem::start(ECS::Registry& ecs)
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

	void TurnBasedSystem::set_targetted_ent(Entity ent)
	{
		targetted_entity = ent;
	}

	void TurnBasedSystem::set_targetted_xy(int x, int y)
	{
		targetted_x = x; targetted_y = y;
	}

	void TurnBasedSystem::next(ECS::Registry & ecs)
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

		gameBoardptr->reset_selected_player();
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

	bool TurnBasedSystem::active()
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

	void TurnBasedSystem::select_card(ECS::Registry& ecs)
	{
		int index = get_selected_cardhand_index();

		Entity current_entt = current();
		Entity card = draw_card(ecs, current_entt, index); //draw_card(ecs, current_entt, participant_hand[index]);

		if (card == NULL_INDEX)
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
		ECS::ComponentTypeID targCompID = ECS::getComponentTypeID<Components::Targetting_Component>();
		if (!ecs.getBitMask()[card].test(targCompID))
		{
			std::cout << "This card does not have targetting" << std::endl;
			return;
		}

		Components::Targetting_Component* targComp = ecs.getComponent<Components::Targetting_Component>(card);
		
		if(targComp->targetting_type == Components::Targetting::SINGLE_TARGET)
			gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::GRID_SELECT);
		else 
			gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::AOE_GRID_SELECT);
		//gbsptr->debug_print();
	}

	//return the cardID inside the hand
	Entity TurnBasedSystem::draw_card(ECS::Registry& ecs, Entity player, size_t chIndex)
	{
		Components::Card_Storage* player_storage = ecs.getComponent<Components::Card_Storage>(player);

		return player_storage->data_card_hand[chIndex];
	}

	//returns the status of target being attacked
	PC_RETURN_TAG TurnBasedSystem::play_card(ECS::Registry& ecs, Entity player,Entity target, int index)
	{
		PC_RETURN_TAG ret = PC_RETURN_TAG::INVALID;

		Entity cardID = this->draw_card(ecs, player, index);

		f32& card_cost = ecs.getComponent<Components::Card_Cost>(cardID)->value;
		int& player_curMana = ecs.getComponent<Components::TurnBasedStats>(player)->points;

		if (card_cost > player_curMana)
		{
			std::cout << "Not enough mana!!" << std::endl;
			return PC_RETURN_TAG::INVALID;
		}

		//if (target != NULL_INDEX)
		//{
		//	Components::CardTag* tag = ecs.getComponent<Components::CardTag>(cardID);
		//	switch (*tag)
		//	{
		//	case Components::CardTag::ATTACK:
		//	{
		//		// cannot hit your self
		//		if (target == current())
		//		{
		//			std::cout << "Cannot hit yourself" << std::endl;
		//			return PC_RETURN_TAG::INVALID;
		//		}

		//		ECS::ComponentTypeID card_value_ID = ECS::getComponentTypeID<Components::Card_Value>();
		//		if (!ecs.getBitMask()[cardID].test(card_value_ID))
		//		{
		//			std::cout << "Selected card doesn't have card_data component";
		//			return PC_RETURN_TAG::INVALID;
		//		}

		//		f32 card_damage = ecs.getComponent<Components::Card_Value>(cardID)->value;

		//		if (Call_AttackSystem(ecs,target,card_damage) == COMBAT_SYSTEM_RETURN_TAG::DIED)
		//			ret = PC_RETURN_TAG::DIED;
		//		else
		//			ret = PC_RETURN_TAG::VALID;
		//		break;
		//	}
		//	default:
		//		break;
		//	}
		//}
		
		//run the function related to the card
		//example 

		//remove the card that just played inside tbs
		player_curMana -= card_cost;
		remove_card(ecs,player,index);
		return ret;
	}

	//DEBUG PRINT
	void TurnBasedSystem::debug_print(ECS::Registry& ecs) const
	{
		//print out info every action
		std::cout << "======================Turn based Stats======================" << std::endl;
		//show deck cards
		show_deck(ecs);
		//Entity cur = current();
		show_hand(ecs);
		show_discard(ecs);
		show_stats(ecs);
		show_HP(ecs);
		std::cout << "============================================================" << std::endl;
	}

	void TurnBasedSystem::show_HP(ECS::Registry& ecs) const
	{
		for (size_t i = 0; i < participants.size(); ++i)
		{
			f32 HP = ecs.getComponent<Components::HP>(participants[i])->c_value;
			char const* name = ecs.getComponent<Components::Name>(participants[i])->value;
			std::cout << name << "'s HP : " << HP << " | " << std::endl;
		}
	}

	void TurnBasedSystem::show_deck(ECS::Registry& ecs) const
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

	void TurnBasedSystem::show_hand(ECS::Registry& ecs) const
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

	void TurnBasedSystem::show_discard(ECS::Registry& ecs) const
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

	void TurnBasedSystem::show_stats(ECS::Registry& ecs) const
	{
		std::cout << "---------Player's Stats---------" << std::endl;
		Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(this->current());
		std::cout << "Points : " << stats->points << '/' << stats->maxPoints << std::endl;
		std::cout << "SPD : " << stats->cur_movSpd << std::endl;
		std::cout << "Shields : " << stats->shields << std::endl;
		std::cout << "--------------------------------" << std::endl;
	}

	void TurnBasedSystem::check_input(ECS::Registry& ecs)
	{
		const std::array<u8, 6> keys = { AEVK_1 ,AEVK_2,AEVK_3,AEVK_4,AEVK_5,AEVK_6 };
		for (int i = 0; i < keys.size(); ++i)
		{
			if (AEInputCheckTriggered(keys[i]))
			{
				select_hand_index(static_cast<size_t>(keys[i] - AEVK_0 - 1));
				
				break;
			}
		}
		if (AEInputCheckTriggered(AEVK_U))
		{
			select_card(ecs);
		}
	}

	void TurnBasedSystem::DrawPhase_add_card(ECS::Registry& ecs)
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
		System::add_card_player_hand(ecs, playerID, card);	//add a random card
		//remove the card afterwards
		deck.erase(deck.begin() + index);
	}

	void TurnBasedSystem::remove_card(ECS::Registry& ecs,Entity user,int index)
	{
		System::remove_card_player(ecs, user, index);	//this is to remove data from ecs
		cardHandptr->remove_card(ecs,index);		//this is for visual side
	}
	//=================================Update===========================================

	//Turn based system's update loop
	void TurnBasedSystem::update(ECS::Registry& ecs)
	{
		// ================= CONSOLE LOG of TBS =================
		// Check for TBS status
		if (is_active)
		{
			update_GBPhasetriggered();
			update_GBPhaseUpdate();
		}
		//============================================================
	}

	void TurnBasedSystem::update_GBPhasetriggered()
	{
		//
		int index = static_cast<int>(gbsptr->getGBPhase());
		int prev_index = index - 1;
		if (prev_index < 0) prev_index = 0;
		bool& triggered = gbsptr->GBPTriggered()[index];

		//check for gameboard phase and react accordingly
		if (triggered)
		{
			switch (gbsptr->getGBPhase())
			{
			case PhaseSystem::GBPhase::START_PHASE:
			{
				//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				gbsptr->GBPTriggered()[index] = false;

				//=============rest shit for player in start phase===============

				Components::TurnBasedStats* stats = ecsptr->getComponent<Components::TurnBasedStats>(playerID);
				stats->cur_movSpd = stats->max_movSpd;
				stats->points = stats->maxPoints;
				stats->shields = 0.f;

				//===============================================================

				gbsptr->GBPActive()[prev_index] = false;
				gbsptr->GBPActive()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::STANDBY_PHASE:
			{
				//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				gbsptr->GBPTriggered()[index] = false;
				gbsptr->GBPActive()[prev_index] = false;
				gbsptr->GBPActive()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::DRAW_PHASE:
			{
				//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				gbsptr->GBPTriggered()[index] = false;
				gbsptr->GBPActive()[prev_index] = false;
				gbsptr->GBPActive()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::MAIN_PHASE:
			{
				//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				gbsptr->GBPTriggered()[index] = false;
				
				debug_print(*ecsptr);

				gbsptr->GBPActive()[prev_index] = false;
				gbsptr->GBPActive()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::PLAYER_RESOLUTION:
			{
				//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				gbsptr->GBPTriggered()[index] = false;
				gbsptr->GBPActive()[prev_index] = false;
				//===================play card==============================
				if (play_card_triggered)
				{
					play_card_triggered = false;

					PC_RETURN_TAG tag = this->play_card(*ecsptr, this->current(), targetted_entity, this->get_selected_cardhand_index());
					//remove the card that just played inside tbs
					if (tag == PC_RETURN_TAG::DIED)
					{
						//must reset the position on the grid to be null or there will be bugs
						if (targetted_x != -1 && targetted_y != -1) gameBoardptr->get_pos()[targetted_x][targetted_y] = -1;
						this->remove_participant(*ecsptr, targetted_entity);
					}

					if (tag != PC_RETURN_TAG::INVALID)
					{
						this->set_selected_card(false);
					}
					
				gbsptr->GBPActive()[index] = true;
				}
				//=========================================================



				break;
			}
			default: 
			{
				std::cout << "triggered something else";
				gbsptr->GBPTriggered()[index] = false;
				gbsptr->GBPActive()[prev_index] = false;
				gbsptr->GBPActive()[index] = true;
				break; 
			}
			}
			triggered = false;
		}
	}

	void TurnBasedSystem::update_GBPhaseUpdate()
	{ 
		size_t index = static_cast<size_t>(gbsptr->getGBPhase());
		bool& active = gbsptr->GBPActive()[index];

		if (active)
		{
			switch (gbsptr->getGBPhase())
			{
			case PhaseSystem::GBPhase::START_PHASE:
			{
				//std::cout << "Starting" << std::endl;
				gbsptr->nextGBPhase();
				index = static_cast<size_t>(gbsptr->getGBPhase());
				gbsptr->GBPTriggered()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::STANDBY_PHASE:
			{
				//std::cout << "Standing By"<< std::endl;
				gbsptr->nextGBPhase();
				index = static_cast<size_t>(gbsptr->getGBPhase());
				gbsptr->GBPTriggered()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::DRAW_PHASE:
			{
				//draw until max_hand
				for (int i = 0; i < DRAW_COUNT; ++i)
				{
					DrawPhase_add_card(*ecsptr);
				}
				cardHandptr->reset_hand();

				gbsptr->nextGBPhase();
				index = static_cast<size_t>(gbsptr->getGBPhase());
				gbsptr->GBPTriggered()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::MAIN_PHASE:
			{
				if (current() == playerID)
				{
					switch (gbsptr->getPlayerPhase())
					{
					case PhaseSystem::PlayerPhase::PLAYER_EXPLORE:
					{
						check_input(*ecsptr);
						break; //break for PLAYER_EXPLORE
					}
					default:
						break;
					}
				}
				//this is break for the GPhase
				break;
			}
			case PhaseSystem::GBPhase::PLAYER_RESOLUTION:
			{
				//TWAN Do your animations here
				std::cout << "Animating" << std::endl;
				//Whenever u are done do this code to go back
				//instead of press P to end animation state u do this
				//if (AEInputCheckTriggered(AEVK_P))
				//{
				//	gbsptr->GBPActive()[index] = false;
				//	//remove if u want to make the the playerphase not reset when u click on invalid target
				//	gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_EXPLORE);
				//	//gbsptr->debug_print();
				//	evsptr->template_pool[UNHIGHLIGHT_EVENT].triggered = true;

				//	gbsptr->set_GBPhase(PhaseSystem::GBPhase::MAIN_PHASE);
				//	int i = static_cast<int>(gbsptr->getGBPhase());
				//	gbsptr->GBPTriggered()[i] = true;
				//}
				gbsptr->GBPActive()[index] = false;
				//remove if u want to make the the playerphase not reset when u click on invalid target
				gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_EXPLORE);
				//gbsptr->debug_print();
				evsptr->template_pool[UNHIGHLIGHT_EVENT].triggered = true;

				gbsptr->set_GBPhase(PhaseSystem::GBPhase::MAIN_PHASE);
				int i = static_cast<int>(gbsptr->getGBPhase());
				gbsptr->GBPTriggered()[i] = true;
				break;
			}
			default:
			{
				std::cout << "Updating error";
				break;
			}
			}
		}
	}
}