#include "TurnBasedSystem.h"
#include "../system/GridSystem.h"
#include "../util/util.h"
#include <iomanip>
namespace TBS
{

	// End round Helper
	bool TurnBasedSystem::everyone_yielded() const
	{
		return gm_yielded[0] && gm_yielded[1];
	}

	// round start helper forcing it to start yadda yadda
	void TurnBasedSystem::round_start(ECS::Registry& ecs)
	{
		gm_yielded[0] = false;
		gm_yielded[1] = false;

		gm_turn_count = 1;

		// If someone yielded first last round, they start this round
		if (first_yield_set)
			current_gm = first_yielder;

		first_yield_set = false;

		std::cout << "\n=== ROUND " << cur_round << " START ===\n";
		std::cout << "[TBS] GM status:\n";
		std::cout << "  - Player yielded=" << gm_yielded[0] << (current_gm == GM::Player ? "  <-- current" : "") << "\n";
		std::cout << "  - Enemy  yielded=" << gm_yielded[1] << (current_gm == GM::Enemy ? "  <-- current" : "") << "\n";

		std::cout << "[TBS] Turn " << gm_turn_count << " | Round " << cur_round
			<< " | Current GM: " << gm_name(current_gm) << "\n";

		debug_print(ecs);
	}

	void TurnBasedSystem::force_start_if_ready(ECS::Registry& ecs)
	{
		if (!is_active && participants.size() >= 2)
			start(ecs);
	}

	void TurnBasedSystem::init(EventPool& eventPool)
	{
		evsptr = &eventPool;
		//highlight event
		evsptr->pool.push_back(Event{});
		//unhighlight event
		evsptr->pool.push_back(Event{});
		//Entity Die event
		evsptr->pool.push_back(Event{});
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
			if (*(participants.begin() + i) == parti)
			{
				participants.erase(participants.begin() + i);	//remove the target from turn system
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
		std::cout << "[DBG] TBS this ptr = " << this << std::endl; //TEMP
		std::cout << std::left << std::setw(30) << "Participants" <<  ':' << "Entity ID"<< std::endl;

		for (size_t i = 0; i < participants.size(); ++i)
		{
			std::cout << std::left << std::setw(30) << ecs.getComponent<Components::Name>(participants[i])->value << ":(ID = "
				<< participants[i] << ')' << std::endl;
		}

		//Addition
		current_gm = GM::Player;   // or whoever should start by default
		first_yield_set = false;   // no first yielder yet
		gm_yielded[0] = gm_yielded[1] = false;

		cur_round = 1;
		gm_turn_count = 0; // round_start will set to 1

		round_start(ecs);  // <-- THIS is the “print Round 1 immediatelyEfix

	}
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

	void TurnBasedSystem::yield_current()
	{
		if (!is_active) return;

		const int gi = gm_index(current_gm);

		if (!first_yield_set)
		{
			first_yield_set = true;
			first_yielder = current_gm; // first GM to yield gets first next round
		}

		gm_yielded[gi] = true;

		std::cout << "[TBS] " << gm_name(current_gm) << " GM yielded.\n";

	}
	void TurnBasedSystem::next(ECS::Registry & ecs)
	{
		if (!is_active) return;

		if (everyone_yielded())
		{
			++cur_round;
			round_start(ecs);
			return;
		}

		// swap GM
		current_gm = (current_gm == GM::Player) ? GM::Enemy : GM::Player;

		// If the next GM already yielded, and the other GM also yielded -> round end
		if (gm_yielded[gm_index(current_gm)])
		{
			if (everyone_yielded())
			{
				++cur_round;
				round_start(ecs);
				return;
			}

			// Otherwise bounce back to the non-yielded GM
			current_gm = (current_gm == GM::Player) ? GM::Enemy : GM::Player;
		}

		++gm_turn_count;
		++cur_player;
		if (cur_player >= participants.size()) cur_player = 0;

		std::cout << "[TBS] Turn " << gm_turn_count
			<< " | Round " << cur_round
			<< " | Current GM: " << gm_name(current_gm) << "\n";
		//print out info every action
		for (size_t i = 0; i < participants.size(); ++i)
		{
			f32 HP = ecs.getComponent<Components::HP>(participants[i])->value;
			std::cout << ecs.getComponent<Components::Name>(participants[i])->value << "'s HP : " << HP << " | " << std::endl;
		}
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


	Entity TurnBasedSystem::draw_card(ECS::Registry& ecs, Entity player, size_t chIndex)
	{
		ECS::ComponentTypeID cardStorage_ID = ECS::getComponentTypeID<Components::Card_Storage>();
		Components::Card_Storage* player_storage = ecs.getComponent<Components::Card_Storage>(player);

		return player_storage->card_storage[chIndex];
	}
	void TurnBasedSystem::play_card(ECS::Registry& ecs,Entity target, Entity cardID)
	{
		bool target_died = false;

		if (target != NULL_INDEX)
		{
			std::cout << '\n' << ecs.getComponent<Components::Name>(participants[size_t(current_gm)])->value <<
				" used " << ecs.getComponent<Components::Name>(cardID)->value << " on " << ecs.getComponent<Components::Name>(target)->value << std::endl;

			//later check for type of card and call accordingly
			//for now it's attack system

			Entity card_ID = draw_card(ecs, current(), get_selected_cardhand_index());
			f32& card_range = ecs.getComponent<Components::Attack>(card_ID)->range;


			target_died = Call_AttackSystem(ecs, cardID, target);
		}

		if (target_died)
		{
			/*evsptr->pool[ENTITY_DIE_EVENT].triggered = true;
			evsptr->pool[ENTITY_DIE_EVENT].returned_value = target;*/
			remove_participant(ecs, target);
		}
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
		std::cout << "[TBS] GM yields: Player=" << gm_yielded[0]
			<< " Enemy=" << gm_yielded[1] << "\n";

		std::cout << "[TBS] Participants (" << participants.size() << "):\n";
		for (size_t i = 0; i < participants.size(); ++i)
		{
			auto* nm = ecs.getComponent<Components::Name>(participants[i]);

			// Mark which participant corresponds to the current GM
			bool is_current =
				(current_gm == GM::Player && i == 0) ||
				(current_gm == GM::Enemy && i == 1);

			std::cout << "  - " << (nm ? nm->value : "<no name>")
				<< " (ID=" << participants[i] << ")"
				<< (is_current ? "  <-- current GM" : "")
				<< "\n";
		}
	}


	//Turn based system's update loop
	void TurnBasedSystem::update(ECS::Registry& ecs,std::vector<Entity>& entities)
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
			debug_print(ecs);
		}

		if (is_active)
		{

			static int once = 0;
			if (once++ == 0)
			{
				std::cout << "[dbg] tbs active, hotkey block running" << std::endl;
				//print out info every action
				for (size_t i = 0; i < participants.size(); ++i)
				{
					f32 HP = ecs.getComponent<Components::HP>(participants[i])->value;
					std::cout << ecs.getComponent<Components::Name>(participants[i])->value << "'s HP : " << HP << " | " << std::endl;
				}
			}


			if (AEInputCheckTriggered(AEVK_1))
			{
				participant_hand[cur_player] = 0;
			}
			if (AEInputCheckTriggered(AEVK_2))
			{
				participant_hand[cur_player] = 1;
			}
			if (AEInputCheckTriggered(AEVK_3))
			{
				participant_hand[cur_player] = 2;
			}
			if (AEInputCheckTriggered(AEVK_4))
			{
				participant_hand[cur_player] = 3;
			}
			if (AEInputCheckTriggered(AEVK_5))
			{
				participant_hand[cur_player] = 4;
			}
			if (AEInputCheckTriggered(AEVK_6))
			{
				participant_hand[cur_player] = 5;
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

			// y = yield (no more turns this round)
			if (AEInputCheckTriggered(AEVK_Y))
			{
				std::cout << "[hotkey] y = yield\n";
				yield_current();
				next(ecs);
			}
			// u = attack (consume turn)
			//else if (AEInputCheckTriggered(AEVK_U))
			//{
			//	Entity current_entt = current();
			//	std::cout << "[hotkey] u = attack\n";
			//	Entity card = draw_card(ecs, current_entt, participant_hand[index]);
			//	std::cout << "Select Enemy to use card on" << std::endl;
			//	selected_card[index] = true;
			//	/*play_card(ecs, card);
			//	next(ecs);*/
			//}
			//implemented to twan's move entity
			// i = move (consume turn)
			/*else if (AEInputCheckTriggered(AEVK_I))
			{
				std::cout << "[hotkey] i = move\n";
				
			}*/
		}
		//============================================================
	}
}