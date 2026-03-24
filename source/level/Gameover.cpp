#include "pch.h"
#include "level/Gameover.h"
//#include "../UI/UI.h"
#include "../util/GameStateManager.h"
#include "../util/LevelManager.h"
#include "../UI/UIObject.h"

void onClick_Restart()
{
	gGameStateNext = GameStates::GS_Game;
	gLevelStateNext = LevelStates::LS_COMBAT;
}

void onClick_Quit()
{
	gGameStateNext = GameStates::GS_MAINMENU;
}

Entity restart, quit;
Entity res_text, quit_text;

void GameState_GameOver_load()
{
	restart = UIO::ui_button_texture(TF.getTextureUI(8), 0.f, 0.f, 225.f, 65.f, 0.f, 30, [] {onClick_Restart();});
	res_text = UIO::ui_text(0.f, 0.f, 225.f, 65.f, 0.f, 34, "Restart"); // <<<<<< This shit doesn t work
	quit = UIO::ui_button_texture(TF.getTextureUI(8), 0.f, -200.f, 225.f, 65.f, 0.f, 30, [] {onClick_Quit(); });
	quit_text = UIO::ui_text(0.f, -200.f, 225.f, 65.f, 0.f, 34, "Quit");  // <<<<<< This shit doesn t work
}
void GameState_GameOver_init()
{

}
void GameState_GameOver_update()
{
	
}
void GameState_GameOver_free()
{
	ecs.destroyEntity(restart);
	ecs.destroyEntity(quit);
	ecs.destroyEntity(res_text);
	ecs.destroyEntity(quit_text);
}
void GameState_GameOver_unload()
{

}

