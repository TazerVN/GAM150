#pragma once
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"

//System that handles targetting of cards
class TargettingSystem
{	
	ECS::Registry* ecs;		//reference to the ecs
	Entity owner{ NULL_INDEX };	//pointer to the owner

	TargettingSystem(ECS::Registry& ecs,Entity owner)
	{
		ECS::ComponentTypeID card_storage_ID = ECS::getComponentTypeID<Components::Card_Storage>();
		this->ecs = &(ecs);
		this->owner = owner;
	}
	bool valid_target(Entity card, Entity target)
	{
		//check if the owner is not null
		if (owner == NULL_INDEX) return;

	}
};