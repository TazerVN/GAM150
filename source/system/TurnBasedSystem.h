#pragma once
#include <vector>
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
namespace TBS 
{
	class TurnBasedSystem
	{
	private:
		bool is_active{ false };
		size_t cur_round{0};
		size_t index{0};
		std::vector<Entity> participants;
	public:
		void add_participant(Entity parti,ECS::Registry ecs) 
		{
			ECS::ComponentTypeID tbs_id = ECS::getComponentTypeID<Components::TurnBasedStats>();

			if (!(ecs.getBitMask()[parti].test(tbs_id)))
			{
				std::cout << "Error entity without TurnBasedStats cannot be added to the turn order!!" << std::endl;
				return;
			}
			participants.push_back(parti);
		}
		void start()
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
		}
		Entity current()
		{
			return participants[index];
		}

		void next()
		{
			index++;
			if (index >= participants.size())
			{
				index = 0;
				cur_round++;
			}
		}

		void end()
		{
			is_active = false;
			cur_round = 0;
			index = 0;
			participants.clear();
		}

		bool active()
		{
			return is_active;
		}

		size_t round()
		{
			return cur_round;
		}
	};
}