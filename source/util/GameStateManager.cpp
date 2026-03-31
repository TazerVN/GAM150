#include "pch.h"
#include "GameStateManager.h"
#include "../level/game.h"
#include "../level/menu.h"
#include "../level/Gameover.h"
#include <iostream>

GameStates gGameStateInit;
GameStates gGameStateCurr;
GameStates gGameStatePrev;
GameStates gGameStateNext;

void (*GameStateLoad)() = 0;
void (*GameStateInit)() = 0;
void (*GameStateUpdate)() = 0;
void (*GameStateFree)() = 0;
void (*GameStateUnload)() = 0;

void GameStateMgrInit(GameStates gameStateInit)
{
	// set the initial game state
	gGameStateInit = gameStateInit;

	// reset the current, previoud and next game
		gGameStateCurr =
		gGameStatePrev =
		gGameStateNext = gGameStateInit;

	// call the update to set the function pointers
	GameStateMgrUpdate();
}

void GameStateMgrUpdate()
{
	if ((gGameStateCurr == GameStates::GS_RESTART) || (gGameStateCurr == GameStates::GS_QUIT))
		return;

	switch (gGameStateCurr)
	{
	case GameStates::GS_MAINMENU:
		GameStateLoad = GameStateMainMenu_load;
		GameStateInit = GameStateMainMenu_init;
		GameStateUpdate = GameStateMainMenu_update;
		GameStateFree = GameStateMainMenu_free;
		GameStateUnload = GameStateMainMenu_unload;
		break;
	case GameStates::GS_Game : 
	{
		GameStateLoad = GameState_game_load;
		GameStateInit = GameState_game_init;
		GameStateUpdate = GameState_game_update;
		GameStateFree = GameState_game_free;
		GameStateUnload = GameState_game_unload;
		break;
	}
	case GameStates::GS_GAMEOVER:
	{
		GameStateLoad  = GameState_GameOver_load;
		GameStateInit = GameState_GameOver_init;
		GameStateUpdate = GameState_GameOver_update;
		GameStateFree = GameState_GameOver_free;
		GameStateUnload = GameState_GameOver_unload;
		break;
	}
	default:
		std::cout << "invalid state!!" << '\n';;
	}
}