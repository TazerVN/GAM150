#include "pch.h"

#include "IntentionDisplay.h"
#include "../system/EnemyDirector.h"

void IntentionDisplaySystem::init(EnemyDirector& enemyDirector)
{
	this->ptr_enemyDirector = &enemyDirector;

	for (auto it = this->ptr_enemyDirector->get_map().begin(); it != this->ptr_enemyDirector->get_map().end(); ++it)
	{
		Entity enemy = it->second;
	}
}

void IntentionDisplaySystem::update()
{
	EntityComponent::ComponentTypeID meshID = EntityComponent::getComponentTypeID<Components::Mesh>();
	EntityComponent::ComponentTypeID textureID = EntityComponent::getComponentTypeID<Components::Texture>();
	EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
	/*EntityComponent::ComponentTypeID intentionID = EntityComponent::getNextComponentTypeID<Components::Tag> */
	//create bitsets
	EntityComponent::ComponentBitMask objMask;
	objMask.set(transID); objMask.set(meshID); objMask.set(textureID);

	for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
	{
		if ((it->first & objMask) == objMask)
		{
			for (Entity ent : it->second)
			{
				Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(ent);
				std::pair<s8, Entity> a{ mesh->z, ent };
			}
		}
	}
}