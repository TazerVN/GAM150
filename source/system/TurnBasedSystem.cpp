#include "TurnBasedSystem.h"
#include "../system/GridSystem.h"
#include "../util/util.h"
#include "../UI/cardInteraction.h"
#include "../global.h"
#include <iomanip>
namespace TBS
{
	// round start helper forcing it to start yadda yadda
	void TurnBasedSystem::round_start(ECS::Registry& ecs)
	{
		cur_player = 0;
		gbsptr->resetGPhase();
		gbsptr->GBPTriggered()[static_cast<size_t>(gbsptr->getGBPhase())] = true;
		gbsptr->resetPlayerPhase();
		debug_print(ecs);	//this is for turn base
	}
	void TurnBasedSystem::round_end()
	{
		++cur_round;
	}
	void TurnBasedSystem::force_start_if_ready(ECS::Registry& ecs)
	{
		if (!is_active && participants.size() >= 2) start(ecs);
	}

	void TurnBasedSystem::init(ECS::Registry& ecs, EventPool& eventPool, Grid::GameBoard& gbp, PhaseSystem::GameBoardState& gbsp, 
		System::CardSystem& cs, CardInteraction::CardHand& ch, std::vector<Entity>& entities)
	{
		evsptr = &eventPool;
		//highlight event
		evsptr->pool.push_back(Event{});
		//unhighlight event
		evsptr->pool.push_back(Event{});
		//GBPhase next event
		evsptr->pool.push_back(Event{});
		//playcard event
		evsptr->pool.push_back(Event{});

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
				std::cout << "Participant size :" << participants.size() << std::endl;
				std::cout << "Paricipant hand size : " << participant_hand.size() << std::endl;
				//kill the removed participant
				ecs.destroyEntity(parti);
			}
		}
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
		std::cout << "Selected card at index " << index << std::endl;;
	}

	void TurnBasedSystem::select_card(ECS::Registry& ecs)
	{
		int index = get_selected_cardhand_index();

		Entity current_entt = current();
		std::cout << "[hotkey] u = attack\n";
		Entity card = draw_card(ecs, current_entt, index); //draw_card(ecs, current_entt, participant_hand[index]);

		if (card == NULL_INDEX)
		{
			std::cout << "Cannot select invalid index" << std::endl;
			return;
		}

		std::cout << "Attacking with " << ecs.getComponent<Components::Name>(card)->value << std::endl;
		std::cout << "Select Enemy to use card on" << std::endl;
		set_selected_card(true);	//set current participant's selected card to true
		evsptr->pool[HIGHLIGHT_EVENT].triggered = true;
		gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::GRID_SELECT);
		gbsptr->debug_print();
	}

	//return the cardID inside the hand
	Entity TurnBasedSystem::draw_card(ECS::Registry& ecs, Entity player, size_t chIndex)
	{
		Components::Card_Storage* player_storage = ecs.getComponent<Components::Card_Storage>(player);

		return player_storage->data_card_hand[chIndex];
	}

	//returns the status of target being attacked
	bool TurnBasedSystem::play_card(ECS::Registry& ecs, Entity player,Entity target, int index)
	{
		bool target_died = false;

		if (target != NULL_INDEX)
		{
			Entity cardID = this->draw_card(ecs, player, index);
			std::string name = ecs.getComponent<Components::Name>(participants[size_t(cur_player)])->value;
			std::string targetName = ecs.getComponent<Components::Name>(target)->value;
			std::string cardName = ecs.getComponent<Components::Name>(cardID)->value;

			std::cout << '\n' << name << " used " << cardName << " on " << targetName << std::endl;

			f32& card_range = ecs.getComponent<Components::Attack>(cardID)->range;

			Components::CardTag* tag = ecs.getComponent<Components::CardTag>(cardID);
			switch (*tag)
			{
			case Components::CardTag::ATTACK:
			{
				if (target == current())
				{
					std::cout << "Cannot hit yourself" << std::endl;
					break;
				}
				target_died = Call_AttackSystem(ecs, cardID, player ,target);
				break;
			}
			case Components::CardTag::DEFENSE:
			{
				if (target != current())
				{
					std::cout << "Can only be used on yourself" << std::endl;
					break;
				}
			}
			default:
				break;
			}

		}
		//remove the card that just played
		remove_card(ecs,player,index);

		show_HP(ecs);
		show_hand(ecs);
		return target_died;
	}

	bool TurnBasedSystem::Call_AttackSystem(ECS::Registry& ecs, Entity cardID, Entity player, Entity target)
	{
		bool ret = false;
		//attack component
		ECS::ComponentTypeID atkID = ECS::getComponentTypeID<Components::Attack>();
		ECS::ComponentTypeID hpID = ECS::getComponentTypeID<Components::HP>();
		//test if card have attack id
		if (!ecs.getBitMask()[cardID].test(atkID)) return false;
		if (!(ecs.getBitMask()[target].test(hpID))) return false;

		//if the have components then reduce the HP amount
		ecs.getComponent<Components::HP>(target)->value -= ecs.getComponent<Components::Attack>(cardID)->damage;
		if (ecs.getComponent<Components::HP>(target)->value <= 0.f) ret = true;

		return ret;
	}


	void TurnBasedSystem::Call_DefenseSystem(ECS::Registry& ecs, Entity cardID, Entity target)
	{
		ECS::ComponentTypeID hpID = ECS::getComponentTypeID<Components::HP>();

		if (!(ecs.getBitMask()[target].test(hpID))) return;
	}
	//DEBUG PRINT
	void TurnBasedSystem::debug_print(ECS::Registry& ecs) const
	{
		//gbsptr->debug_print();
		std::cout << "\n=== ROUND " << cur_round << " START ===\n";
		char const* nm = ecs.getComponent<Components::Name>(participants[cur_player])->value;
		std::cout << nm << " Turn" << std::endl;
		//print out info every action
		show_HP(ecs);
		//Entity cur = current();
		show_hand(ecs);
	}

	void TurnBasedSystem::show_HP(ECS::Registry& ecs) const
	{
		for (size_t i = 0; i < participants.size(); ++i)
		{
			f32 HP = ecs.getComponent<Components::HP>(participants[i])->value;
			char const* name = ecs.getComponent<Components::Name>(participants[i])->value;
			std::cout << name << "'s HP : " << HP << " | " << std::endl;
		}
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

	void TurnBasedSystem::add_card(ECS::Registry& ecs)
	{
		//temporary code random card from the card pool
		int index = int(AERandFloat() * cardSysptr->size());

		Entity card = cardSysptr->get_card(index);

		System::add_card_player(ecs, playerID, card);	//add a random card

		show_hand(ecs);
	}

	void TurnBasedSystem::remove_card(ECS::Registry& ecs,Entity user,int index)
	{
		System::remove_card_player(ecs, user, index);
		cardHandptr->remove_card(ecs,index);
		show_hand(ecs);
	}
	//=================================Update===========================================

	//Turn based system's update loop
	void TurnBasedSystem::update(ECS::Registry& ecs)
	{
		// ================= CONSOLE LOG of TBS =================
		// Check for TBS status
		if (is_active)
		{
			if (AEInputCheckTriggered(AEVK_RSHIFT))
			{
				gbsptr->nextGBPhase();
				gbsptr->GBPTriggered()[static_cast<size_t>(gbsptr->getGBPhase())] = true;
			}

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
				std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				gbsptr->GBPTriggered()[index] = false;
				gbsptr->GBPActive()[prev_index] = false;
				gbsptr->GBPActive()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::STANDBY_PHASE:
			{
				std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				gbsptr->GBPTriggered()[index] = false;
				gbsptr->GBPActive()[prev_index] = false;
				gbsptr->GBPActive()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::DRAW_PHASE:
			{
				std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				gbsptr->GBPTriggered()[index] = false;
				gbsptr->GBPActive()[prev_index] = false;
				gbsptr->GBPActive()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::MAIN_PHASE:
			{
				std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				gbsptr->GBPTriggered()[index] = false;
				gbsptr->GBPActive()[prev_index] = false;
				gbsptr->GBPActive()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::PLAYER_RESOLUTION:
			{
				std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				gbsptr->GBPTriggered()[index] = false;
				gbsptr->GBPActive()[prev_index] = false;

				//===================play card==============================
				
				bool died = this->play_card(*ecsptr, this->current(),targetted_entity, this->get_selected_cardhand_index());
				if (died)
				{
					//must reset the position on the grid to be null or there will be bugs
					if (targetted_x != -1 && targetted_y != -1) gameBoardptr->get_pos()[targetted_x][targetted_y] = -1;
					this->remove_participant(*ecsptr, targetted_entity);
				}
				this->set_selected_card(false);
				gbsptr->prevGBPhase();
				gbsptr->GBPTriggered()[prev_index] = true;
				gbsptr->nextPlayerPhase();
				gbsptr->debug_print();
				evsptr->pool[UNHIGHLIGHT_EVENT].triggered = true;
				break;
				//=========================================================
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
				std::cout << "Starting" << std::endl;
				gbsptr->nextGBPhase();
				index = static_cast<size_t>(gbsptr->getGBPhase());
				gbsptr->GBPTriggered()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::STANDBY_PHASE:
			{
				std::cout << "Standing By"<< std::endl;
				gbsptr->nextGBPhase();
				index = static_cast<size_t>(gbsptr->getGBPhase());
				gbsptr->GBPTriggered()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::DRAW_PHASE:
			{
				std::cout << "Drawing cards" << std::endl;
	
				//draw until max_hand
				for (int i = 0; i < MAX_HAND; ++i)
				{
					add_card(*ecsptr);
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
						break; //break for PLAYER_EXPLORE
					}
					case PhaseSystem::PlayerPhase::CARD_SELECT:
					{
						check_input(*ecsptr);
						break; //break for CARD_SELECT
					}
					default:
						break;
					}
				}
				//this is break for the GPhase
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