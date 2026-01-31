#pragma once
#include <vector>
#include "../ECS/ECSystem.h"
namespace TBS 
{
	class TurnBasedSystem
	{
	private:
		bool is_active{ false };
		bool ended{ false };
		size_t cur_round{0};
		size_t index{0};
		std::vector<Entity> participants;
	public:
		void add_participant(Entity part);

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
	};
}