//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
// 
//	EMAIL:		w.phyooo@digipen.edu, 
// 
//	DATE:		5-4-2026
//===============================================================

#include "LevelManager.h"
#include "../level/CombatLevel.h"
#include "../level/EncounterLevel.h"
#include "../level/TutorialLevel.h"
#include <iostream>

bool init_triggered = true;

LevelStates gLevelStateInit;
LevelStates gLevelStateCurr;
LevelStates gLevelStatePrev;
LevelStates gLevelStateNext;

void (*LevelStateLoad)() = 0;
void (*LevelStateInit)() = 0;
void (*LevelStateUpdate)() = 0;
void (*LevelStateFree)() = 0;
void (*LevelStateUnload)() = 0;

void LevelMgrInit(LevelStates initial_level)
{
	// set the initial game state
	gLevelStateInit = initial_level;

	// reset the current, previoud and next game
	gLevelStateCurr =
	gLevelStatePrev =
	gLevelStateNext = initial_level;

	// call the update to set the function pointers
	LevelMgrUpdate();
}

void LevelMgrUpdate()
{
	if ((gLevelStateCurr == LevelStates::LS_RESTART) || (gLevelStateCurr == LevelStates::LS_QUIT))
		return;

	switch (gLevelStateCurr)
	{
	case LevelStates::LS_COMBAT:
	{
		LevelStateLoad = LevelStateCombat_load;
		LevelStateInit = LevelStateCombat_init;
		LevelStateUpdate = LevelStateCombat_update;
		LevelStateFree = LevelStateCombat_free;
		LevelStateUnload = LevelStateCombat_unload;
		break;
	}
	case LevelStates::LS_ENCOUNTER:
	{
		LevelStateLoad = LevelStateEncounter_load;
		LevelStateInit = LevelStateEncounter_init;
		LevelStateUpdate = LevelStateEncounter_update;
		LevelStateFree = LevelStateEncounter_free;
		LevelStateUnload = LevelStateEncounter_unload;
		break;
	}
	case LevelStates::LS_BOSS:
	{
		LevelStateLoad = LevelStateCombat_load;
		LevelStateInit = LevelStateCombat_init;
		LevelStateUpdate = LevelStateCombat_update;
		LevelStateFree = LevelStateCombat_free;
		LevelStateUnload = LevelStateCombat_unload;
		break;
	}
	case LevelStates::LS_TUTORIAL:
	{
		LevelStateLoad = LevelStateTutorial_load;
		LevelStateInit = LevelStateTutorial_init;
		LevelStateUpdate = LevelStateTutorial_update;
		LevelStateFree = LevelStateTutorial_free;
		LevelStateUnload = LevelStateTutorial_unload;
		break;
	}
	default:
		std::cout << "invalid state!!" << '\n';;
	}
}