#include "pch.h"

#include "../global.h"
#include "../util/LevelManager.h"
#include "../ECS/Scene.h"
#include "../UI/UI.h"
#include "../UI/cardInteraction.h"

s8 pFont; char pText[40];
AEGfxTexture* floortext;
AEGfxTexture* cardtext;

void GameState_game_load()
{
}

void GameState_game_init()
{
	LevelMgrInit(LevelStates::LS_COMBAT);
}

void GameState_game_update()
{
	//==============(Logic Update)==============

	if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		gLevelStateNext = LevelStates::LS_QUIT;
	
	IM.update(ecs, CS.id());
	TS.update(ecs);

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

		if (gLevelStateCurr == gLevelStateNext)
		{
			LevelStateUpdate();
		}
		else
		{
			LevelStateFree();
			init_triggered = true;
			if (gLevelStateNext != LevelStates::LS_RESTART)
			{
				LevelStateUnload();
			}
			gLevelStatePrev = gLevelStateCurr;
			gLevelStateCurr = gLevelStateNext;

		}
	}
	//if LS_QUIT is set
	else
	{
		LevelStateFree();
		init_triggered = true;
		gGameStateNext = GameStates::GS_MAINMENU;
	}

	//========(Render)====================
	RM.RM_render(ecs, CS.id());
	//AEGfxPrint(pFont, pText, 0.f, 0.f, 0.4, 0.f, 0.f, 0.f, 1.f);
}
void GameState_game_free()
{
	init_triggered = true;
	AEGfxDestroyFont(pFont);
}

void GameState_game_unload()
{

}