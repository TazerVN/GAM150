#pragma once

enum class TutorialFlowStage
{
	BASICS = 0,
	MOVEMENT,
	ATTACK_CARD,
	DEFENSE_CARD,
	ITEM_CARD,
	EVENT_CARD,
	DONE
};

extern TutorialFlowStage gTutorialStage;
extern int gTutorialSubstep;

void LevelStateTutorial_load();
void LevelStateTutorial_init();
void LevelStateTutorial_update();
void LevelStateTutorial_free();
void LevelStateTutorial_unload();