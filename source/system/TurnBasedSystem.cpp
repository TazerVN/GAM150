#include "TurnBasedSystem.h"
#include "../system/GridSystem.h"
#include "../util/util.h"
#include "../global.h"
#include <iomanip>
namespace TBS
{
	// round start helper forcing it to start yadda yadda
	void TurnBasedSystem::round_start(ECS::Registry& ecs)
	{
		cur_player = 0;

		debug_print(ecs);	//this is for turn base
	}
	void TurnBasedSystem::round_end()
	{
		++cur_round;
	}
	void TurnBasedSystem::force_start_if_ready(ECS::Registry& ecs)
	{
		if (!is_active && participants.size() >= 2)
			start(ecs);
	}

	void TurnBasedSystem::init(EventPool& eventPool, Grid::GameBoard& gbp, PhaseSystem::GameBoardState& gbsp, System::CardSystem& cs)
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

		gbsptr = &gbsp;
		cardSysptr = &cs;
		gameBoardptr = &gbp;

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
	
		force_start_if_ready(ecs);
	}

	void TurnBasedSystem::remove_participant(ECS::Registry& ecs, Entity parti)
	{
		for (int i = 0; i < participants.size(); ++i)
		{
			if (participants[i] == parti)
			{
				participants.erase(participants.begin() + i);	//remove the target from turn system
				participant_hand.erase(participant_hand.begin() + i);
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

	//current index of participant
	Entity TurnBasedSystem::current()
	{
		// If called unsafely, avoid crashing; return 0-ish
		// (If your Entity type is not integer-like, tell me and we’ll adjust.)
		if (!is_active || participants.empty() || cur_player >= participants.size())
			return Entity{};

		return participants[cur_player];
	}

	Entity TurnBasedSystem::get_selected_cardhand_index()
	{
		return participant_hand[cur_player];
	}

	bool TurnBasedSystem::is_current_selected_card()
	{
		return selected_card[cur_player];
	}

	void TurnBasedSystem::set_selected_card(bool bol)
	{
		selected_card[cur_player] = bol;
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

	std::vector<size_t>& TurnBasedSystem::hand()
	{
		return participant_hand;
	}

	size_t TurnBasedSystem::round()
	{
		return cur_round;
	}

	void TurnBasedSystem::select_card(size_t index)
	{
		participant_hand[cur_player] = index;
		std::cout << "Selected card at index " << index << std::endl;;
	}

	Entity TurnBasedSystem::draw_card(ECS::Registry& ecs, Entity player, size_t chIndex)
	{
		ECS::ComponentTypeID cardStorage_ID = ECS::getComponentTypeID<Components::Card_Storage>();
		Components::Card_Storage* player_storage = ecs.getComponent<Components::Card_Storage>(player);

		return player_storage->card_storage[chIndex];
	}
	bool TurnBasedSystem::play_card(ECS::Registry& ecs,Entity target, Entity cardID)
	{
		bool target_died = false;

		if (target != NULL_INDEX)
		{
			std::cout << '\n' << ecs.getComponent<Components::Name>(participants[size_t(cur_player)])->value <<
				" used " << ecs.getComponent<Components::Name>(cardID)->value << " on " << ecs.getComponent<Components::Name>(target)->value << std::endl;

			//later check for type of card and call accordingly
			//for now it's attack system

			Entity card_ID = draw_card(ecs, current(), get_selected_cardhand_index());
			f32& card_range = ecs.getComponent<Components::Attack>(card_ID)->range;


			target_died = Call_AttackSystem(ecs, cardID, target);
		}
		show_HP(ecs);
		return target_died;
	}

	bool TurnBasedSystem::Call_AttackSystem(ECS::Registry& ecs, Entity cardID, Entity target)
	{
		bool ret = false;
		//attack component
		ECS::ComponentTypeID atkID = ECS::getComponentTypeID<Components::Attack>();
		//test if card have attack id
		if (!ecs.getBitMask()[cardID].test(atkID)) return false;

		ECS::ComponentTypeID hpID = ECS::getComponentTypeID<Components::HP>();
		if (!(ecs.getBitMask()[target].test(hpID))) return false;

		//if the have components then reduce the HP amount
		ecs.getComponent<Components::HP>(target)->value -= ecs.getComponent<Components::Attack>(cardID)->damage;
		if (ecs.getComponent<Components::HP>(target)->value <= 0.f) ret = true;

		return ret;
	}

	//DEBUG PRINT
	void TurnBasedSystem::debug_print(ECS::Registry& ecs) const
	{
		gbsptr->debug_print();
		std::cout << "\n=== ROUND " << cur_round << " START ===\n";
		char const* nm = ecs.getComponent<Components::Name>(participants[cur_player])->value;
		std::cout << nm << " Turn" << std::endl;
		//print out info every action
		show_HP(ecs);
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
		Components::Card_Storage* playerStorage = ecs.getComponent<Components::Card_Storage>(4);
		size_t sz = playerStorage->card_storage.size();
		for (size_t i = 0; i < sz; ++i)
		{
			Entity cardID = playerStorage->card_storage[i];
			Components::Name* name = ecs.getComponent<Components::Name>(cardID);
			std::cout << ((cardID == -1)? "NULL INDEX" : name->value) << ((i == sz - 1) ? "\n" : " | ");
		}
		std::cout << "------------------------------" << std::endl;
	}

	void TurnBasedSystem::add_card(ECS::Registry& ecs)
	{
		//temporary code random card from the card pool
		int index = int(AERandFloat() * cardSysptr->size());

		Entity card = cardSysptr->get_card(index);

		System::add_card_player(ecs, playerID, card);	//add a random card

		show_hand(ecs);
	}


	//=================================Update===========================================

	//Turn based system's update loop
	void TurnBasedSystem::update(ECS::Registry& ecs, std::vector<Entity>& entities)
	{
		// ================= CONSOLE LOG of TBS =================

		if (!is_active)
		{
			for (size_t i = 0; i < entities.size(); ++i)
			{
				add_participant(ecs, entities[i]);
			}
			std::cout << "[TBS] Masters found. Turn system initiated (WOW THIS IS SO COOL ITS LIKE MY OWN JARVIS!!!).\n";
		}

		//================= TURN-BASED HOTKEYS (TEMP) =================
		static int once = 0;
		if (once++ == 0)
		{
			std::cout << "[DBG] Hotkey section reached\n";
			std::cout << "[DBG] TBS active = " << is_active << "\n";
		}

		// Check for TBS status
		if (AEInputCheckTriggered(AEVK_O))
		{
			std::cout << "[DBG] O pressed. TBS active=" << is_active
				<< " round=" << cur_round << "\n";
		}

		if (is_active)
		{

			static int once = 0;
			if (once++ == 0)
			{
				std::cout << "[dbg] tbs active, hotkey block running" << std::endl;
			}

			if (AEInputCheckTriggered(AEVK_RSHIFT))
			{
				if (current() == playerID)
				{
					gbsptr->nextPlayerPhase();
				}
				else 
				{
					gbsptr->nextGBPhase();
					gbsptr->GBPTriggered()[static_cast<size_t>(gbsptr->getGBPhase())] = true;
				}
			}

			update_GBPhasetriggered();
			update_GBPhaseUpdate(ecs);
		}
		//============================================================
	}

	void TurnBasedSystem::update_GBPhasetriggered()
	{
		//
		size_t index = static_cast<size_t>(gbsptr->getGBPhase());
		bool& triggered = gbsptr->GBPTriggered()[index];

		//check for gameboard phase and react accordingly
		if (triggered)
		{
			switch (gbsptr->getGBPhase())
			{
			case PhaseSystem::GBPhase::START_PHASE:
			{
				std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				break;
			}
			case PhaseSystem::GBPhase::STANDBY_PHASE:
			{
				std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				break;
			}
			case PhaseSystem::GBPhase::DRAW_PHASE:
			{
				std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				break;
			}
			case PhaseSystem::GBPhase::MAIN_PHASE:
			{
				std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				gbsptr->GBPActive()[index] = true;
				break;
			}
			case PhaseSystem::GBPhase::RESOLUTION:
			{
				std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				break;
			}
			case PhaseSystem::GBPhase::ENEMY_PHASE:
			{
				std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
				break;
			}
			default: 
			{
				std::cout << "triggered error";
				break; 
			}
			}
			triggered = false;
		}
	}

	void TurnBasedSystem::update_GBPhaseUpdate(ECS::Registry& ecs)
	{
		size_t index = static_cast<size_t>(gbsptr->getGBPhase());
		bool& active = gbsptr->GBPActive()[index];

		if (active)
		{
			switch (gbsptr->getGBPhase())
			{
			case PhaseSystem::GBPhase::MAIN_PHASE:
			{
				const std::array<u8,6> keys = { AEVK_1 ,AEVK_2,AEVK_3,AEVK_4,AEVK_5,AEVK_6};
				for (int i = 0; i < keys.size(); ++i)
				{
					if (AEInputCheckTriggered(keys[i]))
					{
						select_card(static_cast<size_t>(keys[i] - AEVK_0 - 1));
						break;
					}
				}

				if (AEInputCheckTriggered(AEVK_U))
				{
					Entity current_entt = current();
					std::cout << "[hotkey] u = attack\n";
					Entity card = draw_card(ecs, current_entt, get_selected_cardhand_index()); //draw_card(ecs, current_entt, participant_hand[index]);
					std::cout << "Attacking with " << ecs.getComponent<Components::Name>(card)->value << std::endl;
					std::cout << "Select Enemy to use card on" << std::endl;
					set_selected_card(true);	//selected_card[index] = true;
					evsptr->pool[HIGHLIGHT_EVENT].triggered = true;
				}
				break;
			}
			case PhaseSystem::GBPhase::RESOLUTION:
			{
				std::cout << "Updating " << PhaseSystem::GBPhaseNames[index] << std::endl;
				break;
			}
			case PhaseSystem::GBPhase::ENEMY_PHASE:
			{
				std::cout << "Updating " << PhaseSystem::GBPhaseNames[index] << std::endl;
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