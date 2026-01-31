#include "TurnBasedSystem.h"
#include <iomanip>
namespace TBS
{
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
	}

	void TurnBasedSystem::start(ECS::Registry& ecs)
	{
		//check for edge cases
		if (participants.empty())
		{
			std::cout << "Can't start no participants" << std::endl;
			return;
		}
		else if (participants.size() < 2)
		{
			std::cout << "Need more than 1 participant to start" << std::endl;
			return;
		}

		is_active = true;
		std::cout << "Started Combat" << std::endl;
		std::cout << std::left << std::setw(30) << "Participants" <<  ':' << "Entity ID"<< std::endl;

		for (size_t i = 0; i < participants.size(); ++i)
		{
			std::cout << std::left << std::setw(30) << ecs.getComponent<Components::Name>(participants[i])->value << ":(ID = "
				<< participants[i] << ')' << std::endl;
		}
	}
	Entity TurnBasedSystem::current()
	{
		return participants[index];
	}

	void TurnBasedSystem::next()
	{
		index++;
		if (index >= participants.size())
		{
			index = 0;
			cur_round++;
		}
	}

	void TurnBasedSystem::end()
	{
		is_active = false;
		cur_round = 0;
		index = 0;
		participants.clear();
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
}