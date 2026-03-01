#include "level.h"
#include "AEEngine.h"
#include "../factory/MeshFactory.h"
#include "../factory/TextureFactory.h"
#include "../ECS/gameObject.h"
#include "../ECS/Scene.h"
#include "../UI/cardInteraction.h"
#include "../system/renderSystem.h"
#include "../system/inputSystem.h"
#include "../system/transformSystem.h"
#include "../level/GameState.h"
#include "../UI/UI.h"

float camerax = 0.0f;
float cameray = 0.0f;

s8 pFont; char pText[40];
AEGfxTexture* floortext;
AEGfxTexture* cardtext;

ECS::Registry ecs;
MeshFactory mf{};
Scene scene;

InputSystem::InputManager IM;
UI::UIManager UIM;
TextureFactory::TextureFactory TF;
//initialized event handler before tbs
RenderSystem::RenderSystem RM;
CardInteraction::CardHand card{};


static bool triggered = false;
static Entity attacked_enemy = NULL_INDEX;

void game_init()
{
//==========(Init)======================

	s32 w_width = AEGfxGetWindowWidth();
	s32 w_height = AEGfxGetWindowHeight();

	TF.textureInit();
	mf.MeshFactoryInit();

	AEGfxSetCamPosition(camerax, cameray);
	scene.init(ecs, mf, TF);	
	UIM.init(scene, mf, TF);

	/*grid2D.placeEntity(scene.getECS(), scene.getPlayerID(), 5, 5);
	grid2D.placeEntity(scene.getECS(), scene.getEnemyID(), 3, 2);*/

	card = CardInteraction::CardHand(ecs, mf, -3* w_width/8, -w_height/2, w_width/4, 264 ,scene.getTBS(),scene.getGBS() );
	RM.RenderSystem_init(ecs);

	ecs.remove_empty_groups();
}

void game_update()
{
	AESysFrameStart();
	//==============(Logic Update)==============

	if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		leaveGameState();
	
	IM.update(ecs, scene.getGBS());
	card.update_logic(ecs, scene.getTBS(), mf, TF);
	scene.update();

	//==========Object updates===========


	/*sprintf(pText,"Camera Pos : %.2f,%.2f",camerax,cameray);
	int suc = sprintf_s(pText, "Camera Pos : %.2f,%.2f", camerax, cameray);*/

	//========(Render)====================
	scene.getBattleGrid().update(ecs);
	RM.RM_render(ecs);
	//AEGfxPrint(pFont, pText, 0.f, 0.f, 0.4, 0.f, 0.f, 0.f, 1.f);
	AESysFrameEnd();
}
void game_exit()
{
	mf.MeshFree();
	AEGfxDestroyFont(pFont);
}