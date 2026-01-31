#include "level1.h"
#include "AEEngine.h"
#include "../rendering/Mesh_factory.h"
#include "../rendering/renderSystem.h"
#include "../rendering/TextureFactory.h"
#include "../ECS/gameObject.h"
#include "../ECS/Scene.h"
#include "../cards/cardInteraction.h"
#include "../system/inputSystem.h"
#include "../system/transformSystem.h"
#include "../level/GameState.h"
#include "../system/TurnBasedSystem.h"

float camerax = 0.0f;
float cameray = 0.0f;

s8 pFont; char pText[40];
AEGfxTexture* floortext;
AEGfxTexture* cardtext;
f32 w, h;

Scene level1;

RenderSystem::RenderSystem RM;
TransformSystem::TransformSystem TS;
InputSystem::InputManager IM;
TextureFactory::TextureFactory TF;
TBS::TurnBasedSystem TBSys;
Grid::Grid2D grid2D;

MeshFactory mf{};
CardInteraction::CardHand card{};

//TEMPORARY HELPER WILL MOVE AFTER FEW DAYS (jsut to implement and test the turn based FOR NOW)
static Entity FindEntityByName(ECS::Registry& ecs, const char* name)
{
	ECS::ComponentStorage<Components::Name>* names = ecs.getComponentStorage<Components::Name>();
	ECS::ComponentTypeID nID = ECS::getComponentTypeID<Components::Name>();

	for (int i = 0; i < names->getCount(); ++i)
	{
		if (!ecs.getBitMask()[i].test(nID)) continue;
		auto* nm = ecs.getComponent<Components::Name>(i);
		if (nm && nm->value == name)
			return (Entity)i;
	}

	return (Entity)NULL_INDEX;
}


void game_init()
{
//==========(Init)======================

	s32 w_width = AEGfxGetWindowWidth();
	s32 w_height = AEGfxGetWindowHeight();

	mf.MeshFactoryInit();
	TF.addTexture(AEGfxTextureLoad("../../Assets/cardSample.png"));
	TF.addTexture(AEGfxTextureLoad("../../Assets/floor_01.png"));

	AEGfxSetCamPosition(camerax, cameray);
	pFont = AEGfxCreateFont("../../Assets/liberation-mono.ttf", (int)72.f);

	level1.init(mf);

	////START OF TURNBASED CODE
	//auto& ecs = level1.getECS();

	//// Change these strings to whatever you named them in scene init
	//Entity player = FindEntityByName(ecs, "Player");
	//Entity enemy = FindEntityByName(ecs, "Enemy");

	//// Add if found
	//TBSys.add_participant(ecs, player);
	//TBSys.add_participant(ecs, enemy);
	//TBSys.start(ecs);

	////END

	//ECSystem::Entity Grid = *GameObject::gameobject_grid_create(ecs, mf, 100, 100, 50, 50, 0, 0 ,floortext);
	card = CardInteraction::CardHand(level1.getECS(), mf, -3* w_width/8, -w_height/2, w_width/4, 264, TF.getTexture(0));

	grid2D.init(level1.getECS(),mf,TF.getTexture(1));

	RM.RenderSystem_init(level1.getECS());

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
		//card.update_pos(ecs, TS, camerax, cameray);
		AEGfxSetCamPosition(camerax, cameray);
	}

	level1.update();

	IM.update(level1.getECS());

	// ================= CONSOLE LOG of TBS =================
	auto& ecs = level1.getECS();

	if (!TBSys.active())
	{
		Entity player = FindEntityByName(ecs, "Player1");
		Entity enemy = FindEntityByName(ecs, "Enemy1");

		if (player != Entity{} && enemy != Entity{})
		{
			TBSys.add_participant(ecs, player);
			TBSys.add_participant(ecs, enemy);

			std::cout << "[TBS] Masters found. Turn system initiated (WOW THIS IS SO COOL ITS LIKE MY OWN JARVIS!!!).\n";
		}
	}
	
	// ================= TURN-BASED HOTKEYS (TEMP) =================
	static int once = 0;
	if (once++ == 0)
	{
		std::cout << "[DBG] Hotkey section reached\n";
		std::cout << "[DBG] TBS active = " << TBSys.active() << "\n";
	}

	// Check for TBS status
	if (AEInputCheckTriggered(AEVK_O))
	{
		std::cout << "[DBG] O pressed. TBS active=" << TBSys.active()
			<< " round=" << TBSys.round() << "\n";
		TBSys.debug_print(ecs);
	}

	if (TBSys.active())
	{

		static int once = 0;
		if (once++ == 0) std::cout << "[dbg] tbs active, hotkey block running\n";


		// y = yield (no more turns this round)
		if (AEInputCheckTriggered(AEVK_Y))
		{
			std::cout << "[hotkey] y = yield\n";
			TBSys.yield_current();
			TBSys.next(ecs);
		}
		// u = attack (consume turn)
		else if (AEInputCheckTriggered(AEVK_U))
		{
			std::cout << "[hotkey] u = attack\n";
			TBSys.next(ecs);
		}
		// i = move (consume turn)
		else if (AEInputCheckTriggered(AEVK_I))
		{
			std::cout << "[hotkey] i = move\n";
			TBSys.next(ecs);
		}
	}
	// ============================================================

	//==========Object updates===========


	//sprintf(pText,"Camera Pos : %.2f,%.2f",camerax,cameray);
	//int suc = sprintf_s(pText, "Camera Pos : %.2f,%.2f", camerax, cameray);

	//========(Render)====================



	RM.RM_render(level1.getECS());
	AEGfxPrint(pFont, pText, 0.f, 0.f, 0.4, 0.f, 0.f, 0.f, 1.f);
	AESysFrameEnd();
}
void game_exit()
{
	mf.MeshFree();
	AEGfxTextureUnload(cardtext);
	AEGfxDestroyFont(pFont);
}