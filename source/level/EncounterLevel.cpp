#include "pch.h"
#include "level/EncounterLevel.h"

void LevelStateEncounter_load()
{
	std::cout << "Encounter Loaded!" << std::endl;
}
void LevelStateEncounter_init()
{
	std::cout << "Encounter Loaded!" << std::endl;
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
	std::cout << "Encounter Freed!" << std::endl;
}
void LevelStateEncounter_unload()
{
	std::cout << "Encounter unloaded!" << std::endl;
}