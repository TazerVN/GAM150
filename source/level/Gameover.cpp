//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Pham Minh Tuan
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================
#include "level/Gameover.h"
#include "../util/GameStateManager.h"
#include "../util/LevelManager.h"
#include "../UI/UIObject.h"
#include "../UI/MenuUI.h"
#include "global.h"

void onClick_Quit()
{
	gGameStateNext = GameStates::GS_MAINMENU;
}

Entity quit;
Entity  quit_text;
GameOverUI GOU;


void GameState_GameOver_load()
{
	GOU.init();
	gameData.scoringSystem.reset();
	ecs.getComponent <Components::Card_Storage>(playerID)->reset();
}
void GameState_GameOver_init()
{
	AF.bgm.stopping = false;
	AF.bgm.play(2);
}
void GameState_GameOver_update()
{
	GOU.update();
}
void GameState_GameOver_free()
{
	AF.bgm.stop();
	GOU.free();
}
void GameState_GameOver_unload()
{

}

