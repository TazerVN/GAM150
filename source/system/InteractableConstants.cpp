#include "pch.h"
#include "InteractableConstants.h"
#include "../util/LevelManager.h"

static std::vector<Entity*> nodes;

void goToCombat(Entity* e)
{
	nodes.push_back(e);
	std::cout << "Entity : " << *e << std::endl;
	/*if (gLevelStateCurr == LevelStates::LS_COMBAT) gLevelStateNext = LevelStates::LS_RESTART;
	else gLevelStateNext = LevelStates::LS_COMBAT;*/
}
void goToEncounter()
{
	gLevelStateNext = LevelStates::LS_ENCOUNTER;
}
