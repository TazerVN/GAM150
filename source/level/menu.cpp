#include "menu.h"
#include <iostream>
#include "AEEngine.h"
#include "../util/GameStateManager.h"

void GameStateMainMenu_load()
{
	std::cout << "Main menu loaded" << std::endl;
}
void GameStateMainMenu_init()
{
	std::cout << "Main menu init" << std::endl;
}
void GameStateMainMenu_update()
{
	if (AEInputCheckTriggered(AEVK_1))
		gGameStateNext = GameStates::GS_Game;
	if (AEInputCheckTriggered(AEVK_Q))
		gGameStateNext = GameStates::GS_QUIT;
}
void GameStateMainMenu_free()
{
	std::cout << "Main menu free" << std::endl;
}
void GameStateMainMenu_unload()
{
	std::cout << "Main menu unloaded" << std::endl;
}