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

void GameState_GameOver_load()
{
	UIO::ui_button_texture(TF.getTextureUI(9), 0.9F * AEGfxGetWinMaxX(), 0.85F * AEGfxGetWinMaxY(), 100, 90, 0, 30, [] {onClick_Restart();});
}
void GameState_GameOver_init()
{

}
void GameState_GameOver_update()
{
	/*Components::Text* text = ecs.getComponent<Components::Text>(turn);
	std::string temp = std::to_string(scene.getTBS().round());
	std::string buffer{ "Round " + temp };
	text->text = buffer;*/
}
void GameState_GameOver_free()
{

}
void GameState_GameOver_unload()
{

}

