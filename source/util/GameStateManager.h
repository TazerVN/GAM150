//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
// 
//	EMAIL:		w.phyooo@digipen.edu, 
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once


enum class GameStates
{
	// list of all game states 
	GS_Game =0 ,
	GS_MAINMENU,
	GS_LOGO,
	// special game state IDs
	GS_RESTART,
	GS_GAMEOVER,
	GS_QUIT,
	GS_NUM
};

extern GameStates gGameStateInit;
extern GameStates gGameStateCurr;
extern GameStates gGameStatePrev;
extern GameStates gGameStateNext;

extern void (*GameStateLoad)();
extern void (*GameStateInit)();
extern void (*GameStateUpdate)();
extern void (*GameStateFree)();
extern void (*GameStateUnload)();


void GameStateMgrInit(GameStates gameStateInit);

void GameStateMgrUpdate();