//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
// 
//	EMAIL:		w.phyooo@digipen.edu, 
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once


enum class LevelStates
{
	LS_COMBAT = 0,
	LS_ENCOUNTER = 1,
	LS_BOSS,
	LS_TUTORIAL,
	LS_RESTART,
	LS_QUIT,
	LS_NUM
};

extern bool init_triggered;

extern LevelStates gLevelStateInit;
extern LevelStates gLevelStateCurr;
extern LevelStates gLevelStatePrev;
extern LevelStates gLevelStateNext;

extern void (*LevelStateLoad)();
extern void (*LevelStateInit)();
extern void (*LevelStateUpdate)();
extern void (*LevelStateFree)();
extern void (*LevelStateUnload)();


void LevelMgrInit(LevelStates initial_level);

void LevelMgrUpdate();