#include "TurnBasedSystem.h"
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

	void TurnBasedSystem::add_participant(ECS::Registry& ecs, Entity parti)
	{
		ECS::ComponentTypeID tbs_id = ECS::getComponentTypeID<Components::TurnBasedStats>();

		if (!(ecs.getBitMask()[parti].test(tbs_id)))
		{
			std::cout << "Error entity without TurnBasedStats cannot be added to the turn order!!" << std::endl;
			return;
		}
		participants.push_back(parti);

		std::cout << "Added participant : "<< ecs.getComponent<Components::Name>(parti)->value << std::endl;
	
		force_start_if_ready(ecs);
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
		index = 0;

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

		round_start(ecs);  // <-- THIS is the “print Round 1 immediately” fix

	}
	Entity TurnBasedSystem::current()
	{
		// If called unsafely, avoid crashing; return 0-ish
		// (If your Entity type is not integer-like, tell me and we’ll adjust.)
		if (!is_active || participants.empty() || index >= participants.size())
			return Entity{};

		return participants[index];
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


	void TurnBasedSystem::next(ECS::Registry& ecs)
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

		std::cout << "[TBS] Turn " << gm_turn_count
			<< " | Round " << cur_round
			<< " | Current GM: " << gm_name(current_gm) << "\n";
	}

	void TurnBasedSystem::end()
	{
		is_active = false;
		cur_round = 0;
		index = 0;
		participants.clear();
		std::cout << "[DBG] TBS end() CALLED\n";
	}

	bool TurnBasedSystem::active()
	{
		return is_active;
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
}