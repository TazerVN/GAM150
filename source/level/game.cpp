#include "game.h"
#include "AEEngine.h"
#include "../level/GameState.h"
#include "../rendering/Mesh_factory.h"
#include "../rendering/renderSystem.h"
#include "../ECS/ecs.h"
#include "../ECS/gameObject.h"
#include "../cards/cardInteraction.h"
#include "../system/transformSystem.h"

float camerax = 0.0f;
float cameray = 0.0f;

s8 pFont; char pText[40];
AEGfxTexture* floortext;
AEGfxTexture* cardtext;
f32 w, h;

RenderSystem::RenderSystem RM;
TransformSystem::TransformSystem TS;
ECSystem::ECS ecs{};
MeshFactory mf{};
CardInteraction::CardHand card{};


void game_init()
{
//==========(Init)======================

	f32 w_width = AEGfxGetWindowWidth();
	f32 w_height = AEGfxGetWindowHeight();

	mf.MeshFactoryInit();

	AEGfxSetCamPosition(camerax, cameray);
	pFont = AEGfxCreateFont("Assets/liberation-mono.ttf", (int)72.f);
	floortext = AEGfxTextureLoad("Assets/floor_02.png");
	cardtext = AEGfxTextureLoad("Assets/cardSample.png");

	ECSystem::Entity Grid = *GameObject::gameobject_grid_create(ecs, mf, 100, 100, 50, 50, 0, 0 ,floortext);
	card = CardInteraction::CardHand(ecs, mf, -3* w_width/8, -w_height/2, w_width/4, 264, cardtext);


	// Text to print
	AEGfxGetPrintSize(pFont, pText, 1.f, &w, &h);
}
void game_update()
{
	AESysFrameStart();
	//==============(Logic Update)==============


	if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		leaveGameState();



	f32 dt = (f32)AEFrameRateControllerGetFrameTime();

	int dx = 0, dy = 0;
	f32 spd = 250.0f;

	if (AEInputCheckCurr(AEVK_D))
	{
		dx = 1;
	}
	if (AEInputCheckCurr(AEVK_A))
	{
		dx = -1;
	}
	if (AEInputCheckCurr(AEVK_W))
	{
		dy = 1;
	}
	if (AEInputCheckCurr(AEVK_S))
	{
		dy = -1;
	}
	//if direction is pressed
	if (dx != 0 || dy != 0)
	{
		AEVec2 dir;
		AEVec2Set(&dir, (f32)dx, (f32)dy);
		AEVec2Normalize(&dir, &dir);
		if (dir.x != 0)
		{
			camerax += dir.x * spd * dt;
		}
		if (dir.y != 0)
		{
			cameray += dir.y * spd * dt;
		}
		card.update_pos(ecs, TS, camerax, cameray);
		AEGfxSetCamPosition(camerax, cameray);
	}

	//==========Object updates===========


	//sprintf(pText,"Camera Pos : %.2f,%.2f",camerax,cameray);
	//int suc = sprintf_s(pText, "Camera Pos : %.2f,%.2f", camerax, cameray);

	//========(Render)====================



	RM.RM_render(ecs);
	AEGfxPrint(pFont, pText, 0.f, 0.f, 0.4, 0.f, 0.f, 0.f, 1.f);
	AESysFrameEnd();
}
void game_exit()
{
	mf.MeshFree();
	AEGfxTextureUnload(floortext);
	AEGfxTextureUnload(cardtext);
	AEGfxDestroyFont(pFont);
}