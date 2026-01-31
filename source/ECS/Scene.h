#pragma once
#include "../ECS/ECSystem.h"
#include "../cards/Systems.h"
#include "../system/TurnBasedSystem.h"
#include "AEEngine.h"

class Scene
{
private:
	ECS::Registry ecs;
	System::CardSystem card_system;
	Entity playerID{NULL_INDEX};
	Entity enemyID{NULL_INDEX};
	TBS::TurnBasedSystem tbs;

public:
	void init(MeshFactory& mf);
	void update();
	Entity& getPlayerID();
	Entity& getEnemyID();
	ECS::Registry& getECS();
};