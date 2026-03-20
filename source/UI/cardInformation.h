#pragma once

#include "../ECS/components.h"
#include "../ECS/ECSystem.h"
#include "../System/TurnBasedSystem.h"



class CardDisplay
{
	private:
	Entity current_card_id;
	struct Info
	{
		Entity name;
		Entity cost;
		Entity description;
	};
	Info text;
	bool on;

	public:
	CardDisplay() = default;
	void update(EntityComponent::Registry& ecs);

};

