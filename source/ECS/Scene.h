#pragma once
#include "../ECS/ECSystem.h"
#include "../cards/Systems.h"
#include "AEEngine.h"

class Scene
{
private:
	ECS::Registry ecs;
	System::CardSystem card_system;
	Entity playerID{NULL_INDEX};
	Entity enemyID{NULL_INDEX};
public:
	void init(MeshFactory& mf);
	Entity& getPlayerID();
	Entity& getEnemyID();
	ECS::Registry& getECS();
};