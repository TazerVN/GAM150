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
	//==========System=============
	CS.init(ecs);
	RM.RenderSystem_init(ecs);
	card_system.init_cards(ecs);

	Entity temp;

	//Add player
	temp = EntityFactory::create_actor_spritesheet(ecs, mf, { 0.f,0.f }, { 192.0f,192.0f }, "Player", 100.f, TF.getTextureChar(2), Components::AnimationType::NONE);
	playerID = temp;//important must set the playerID !!!!!!!!!!!
	for (int i = 0; i < 8; ++i)
	{
		EntityFactory::add_card_player_deck(ecs, playerID, card_system.generate_card_from_bible(ecs, "Slash"));
	}
	for (int i = 0; i < 3; ++i)
	{
		EntityFactory::add_card_player_deck(ecs, playerID, card_system.generate_card_from_bible(ecs, "Black Hole"));
	}
	//=============================
}

void GameState_game_init()
{
	s32 w_width = AEGfxGetWindowWidth();
	s32 w_height = AEGfxGetWindowHeight();

	LevelMgrInit(LevelStates::LS_COMBAT);
}

void GameState_game_update()
{
	//==============(Logic Update)==============

	if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		gGameStateNext = GameStates::GS_MAINMENU;
	
	IM.update(ecs, CS.id());
	TS.update(ecs);

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
		if (gLevelStateNext != LevelStates::LS_RESTART)
		{
			LevelStateUnload();
		}

		gLevelStatePrev = gLevelStateCurr;
		gLevelStateCurr = gLevelStateNext;

		if (gLevelStateNext == LevelStates::LS_QUIT)
		{
			gGameStateNext = GameStates::GS_MAINMENU;
		}
		init_triggered = true;
	}

	//========(Render)====================
	VS.update(ecs);
	CS.update(ecs);
	RM.RM_render(ecs, CS.id());
	//AEGfxPrint(pFont, pText, 0.f, 0.f, 0.4, 0.f, 0.f, 0.f, 1.f);
}
void GameState_game_free()
{
	AEGfxDestroyFont(pFont);
}

void GameState_game_unload()
{

}