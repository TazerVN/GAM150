#include "../main.h"
#include "../util/LevelManager.h"

s8 pFont; char pText[40];
AEGfxTexture* floortext;
AEGfxTexture* cardtext;

Scene scene;
CardInteraction::CardHand card{};
UI::UIManager UIM;
Particle::ParticleSystem PS;

void GameState_game_load()
{
	//==========System=============
	CS.init(ecs);
	RM.RenderSystem_init(ecs);
	card_system.init_cards(ecs);
	//=============================
}

void GameState_game_init()
{
	LevelMgrInit(LevelStates::LS_COMBAT);
	LevelStateInit();
}

void GameState_game_update()
{
	//==============(Logic Update)==============

	if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		gGameStateNext = GameStates::GS_MAINMENU;
	
	IM.update(ecs, CS.id());
	TS.update(ecs);

	//=====================DOnt dtouchaskkgasg========================

	if (gLevelStateCurr == gLevelStateNext)
	{
		LevelStateUpdate();
	}
	else
	{
		LevelStateFree();
		if (gLevelStateNext != LevelStates::LS_RESTART)
			LevelStateUnload();

		gLevelStatePrev = gLevelStateCurr;
		gLevelStateCurr = gLevelStateNext;
	}

	//========(Render)====================
	AS.update(ecs);
	VS.update(ecs);
	PS.update(ecs, 0.2);
	CS.update(ecs);
	RM.RM_render(ecs, CS.id());
	//AEGfxPrint(pFont, pText, 0.f, 0.f, 0.4, 0.f, 0.f, 0.f, 1.f);
}
void GameState_game_free()
{
	mf.MeshFree();
	AEGfxDestroyFont(pFont);
}

void GameState_game_unload()
{

}