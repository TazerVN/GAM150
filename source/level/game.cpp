#include "pch.h"

#include "../global.h"
#include "../util/LevelManager.h"
#include "../ECS/Scene.h"
#include "../UI/UI.h"
#include "../UI/cardInteraction.h"
#include "game.h"

//s8 pFont; char pText[40];
//AEGfxTexture* floortext;
//AEGfxTexture* cardtext;


void GameState_game_load()
{
	AF.bgm.play(0);
}

void GameState_game_init()
{
	LevelMgrInit(gLevelStateNext);
}

void GameState_game_update()
{
	//==============(Logic Update)==============

	if (gLevelStateNext != LevelStates::LS_QUIT)
	{
		if (init_triggered)
		{
			init_triggered = false;

			if (gLevelStateCurr != LevelStates::LS_RESTART)
			{
				LevelMgrUpdate();
				LevelStateLoad();
			}
			else
				gLevelStateNext = gLevelStateCurr = gLevelStatePrev;

			LevelStateInit();
		}

		LevelStateUpdate();

		if (player_died && gGameStateCurr != GameStates::GS_GAMEOVER)
		{
			gameData.new_Start = true;
			LevelStateFree();
			LevelStateUnload();
			init_triggered = true;
			gGameStateNext = GameStates::GS_GAMEOVER;
		}
		else if (gLevelStateCurr != gLevelStateNext)
		{
			LevelStateFree();
			LevelStateUnload();
			init_triggered = true;
			gLevelStatePrev = gLevelStateCurr;
			gLevelStateCurr = gLevelStateNext;
		}
	}
	//if LS_QUIT is set
	else	
	{
		gGameStateNext = GameStates::GS_MAINMENU;
	}
}
void GameState_game_free()
{
	init_triggered = true;

	//AEGfxDestroyFont(pFont);
}

void GameState_game_unload()
{
	PS.particle_system_free();
	PUT.free();
	AF.bgm.stop();
	ecs.remove_empty_groups();


	

}