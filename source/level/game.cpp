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

	//===========Game===============
	scene.init(ecs, mf, card_system, TF, CS, card);
	card = CardInteraction::CardHand(ecs, mf,TF, -1 * w_width / 8, -w_height / 2, w_width / 2, 264, scene.getTBS(), scene.getBattleGrid()
		, scene.getGBS());
	UIM.init(scene, mf, TF);
	ecs.remove_empty_groups();
	//.spawn_one(ecs, mf, 0.0f,0.0f, 5.0f, 5.0f, 0.0f, 10); // spawn one particle
	//PS.particleDigitize(ecs, mf);
	PS.particleBurst(ecs, mf);
	AS.init(ecs);
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