//=================================================================================
//	AUTHOR:		Zejin Kendreich Dayap Chen 
// 
//	EMAIL:		chen.z@digipen.edu
// 
//	DATE:		5-4-2026
//=================================================================================
#pragma once

enum class TutorialFlowStage
{
	BASICS = 0,
	MOVEMENT,
	ATTACK_CARD,
	DEFENSE_CARD,
	ITEM_CARD,
	EVENT_CARD,
	WIN_TRANSITION,
	DONE
};

extern TutorialFlowStage gTutorialStage;
extern int gTutorialSubstep;

void LevelStateTutorial_load();
void LevelStateTutorial_init();
void LevelStateTutorial_update();
void LevelStateTutorial_free();
void LevelStateTutorial_unload();