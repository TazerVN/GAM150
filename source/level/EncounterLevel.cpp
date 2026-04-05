//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
// 
//	EMAIL:		w.phyooo@digipen.edu, 
// 
//	DATE:		5-4-2026
//===============================================================

#include "pch.h"
#include "level/EncounterLevel.h"

void LevelStateEncounter_load()
{
	std::cout << "Encounter Loaded!" << '\n';
}
void LevelStateEncounter_init()
{
	std::cout << "Encounter Loaded!" << '\n';
}
void LevelStateEncounter_update()
{
	std::cout << "!";
	if (AEInputCheckTriggered(AEVK_ESCAPE))
	{
		gLevelStateNext = LevelStates::LS_QUIT;
	}
}
void LevelStateEncounter_free()
{
	std::cout << "Encounter Freed!" << '\n';
}
void LevelStateEncounter_unload()
{
	std::cout << "Encounter unloaded!" << '\n';
}