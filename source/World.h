#pragma once
#include "card.h"
#include "Systems.h"
#include "AEEngine.h"

class World
{
private:
	ECS::Registry ecs;
	System::CardSystem card_system;
	Entity playerID{NULL_INDEX};
	Entity enemyID{NULL_INDEX};
public:
	void init();
	Entity& getPlayerID();
	Entity& getEnemyID();
	ECS::Registry& getECS();
};