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
#include "../system/TurnBasedSystem.h"
#include "../UI/UI.h"
#include "../util/Event.h"

float camerax = 0.0f;
float cameray = 0.0f;

s8 pFont; char pText[40];
AEGfxTexture* floortext;
AEGfxTexture* cardtext;
f32 w, h;

MeshFactory mf{};
Scene scene;

TransformSystem::TransformSystem TS;
InputSystem::InputManager IM;
TextureFactory::TextureFactory TF;
//initialized event handler before tbs
EventPool eventPool;
TBS::TurnBasedSystem TBSys(eventPool);
Grid::GameBoard grid2D;
RenderSystem::RenderSystem RM;
CardInteraction::CardHand card{};


void highlight_cells(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb);
AEVec2& Get_CurPart_gridPos(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb);

static bool triggered = false;
static Entity attacked_enemy = NULL_INDEX;

void game_init()
{
//==========(Init)======================

	s32 w_width = AEGfxGetWindowWidth();
	s32 w_height = AEGfxGetWindowHeight();

	mf.MeshFactoryInit();
	TF.textureInit();

	AEGfxSetCamPosition(camerax, cameray);
	pFont = AEGfxCreateFont("../../Assets/liberation-mono.ttf", (int)72.f);

	//ECSystem::Entity Grid = *GameObject::gameobject_grid_create(ecs, mf, 100, 100, 50, 50, 0, 0 ,floortext);
	scene.init(mf, TF);	
	grid2D.init(scene.getECS(),mf,&TBSys, eventPool,TF.getTexture(1), 0, w_height/3);

	std::vector<Entity>& vec = scene.entities_store();

	for (size_t i = 0; i < scene.entities_store().size(); ++i)
	{
		grid2D.placeEntity(scene.getECS(), scene.entities_store()[i], 5 + i * 3, 5);
	}

	/*grid2D.placeEntity(scene.getECS(), scene.getPlayerID(), 5, 5);
	grid2D.placeEntity(scene.getECS(), scene.getEnemyID(), 3, 2);*/

	//Entity hp_bar = UI::hp_bar(scene.getECS(), mf, 0, 0, 1000, 200, 0, 0);
	//Entity hp_bar = UI::hp_bar(scene.getECS(), mf, 0, 0, 1000, 500, 0, 2);

	card = CardInteraction::CardHand(scene.getECS(), mf, -3* w_width/8, -w_height/2, w_width/4, 264, TF.getTexture(0));

	RM.RenderSystem_init(scene.getECS());

	// Text to print
	AEGfxGetPrintSize(pFont, pText, 1.f, &w, &h);
}

void game_update()
{
	AESysFrameStart();
	//==============(Logic Update)==============

	if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		leaveGameState();
	
	scene.update();
	TBSys.update(scene.getECS(),scene.entities_store());

	//==================Handle Events===============================
	if (eventPool.pool[HIGHLIGHT_EVENT].triggered)
	{
		highlight_cells(scene.getECS(), TBSys, grid2D);
		eventPool.pool[HIGHLIGHT_EVENT].triggered = false;
	}

	if (eventPool.pool[ATTACK_EVENT].triggered)
	{
		if (eventPool.pool[ATTACK_EVENT].returned_value == NULL_INDEX) return;
		Entity current_entt = TBSys.current();
		Entity cardID = TBSys.draw_card(scene.getECS(), current_entt, TBSys.get_selected_cardhand_index());
		TBSys.play_card(scene.getECS(), eventPool.pool[ATTACK_EVENT].returned_value, cardID);
		TBSys.set_selected_card(false);
		TBSys.next(scene.getECS());

		eventPool.pool[ATTACK_EVENT].triggered = false;
		eventPool.pool[ATTACK_EVENT].returned_value = NULL_INDEX;
	}
	//==============================================================

	IM.update(scene.getECS());
	card.update(scene.getECS(), TBSys);

	//==========Object updates===========


	/*sprintf(pText,"Camera Pos : %.2f,%.2f",camerax,cameray);
	int suc = sprintf_s(pText, "Camera Pos : %.2f,%.2f", camerax, cameray);*/

	//========(Render)====================


	grid2D.update(scene.getECS());
	RM.RM_render(scene.getECS());
	AEGfxPrint(pFont, pText, 0.f, 0.f, 0.4, 0.f, 0.f, 0.f, 1.f);
	AESysFrameEnd();
}
void game_exit()
{
	mf.MeshFree();
	AEGfxDestroyFont(pFont);
}

void highlight_cells(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb)
{
	//=========================Highlight_cells=================================
	Entity card_ID = TBSys.draw_card(scene.getECS(), tbs.current(), tbs.get_selected_cardhand_index());
	f32& card_range = ecs.getComponent<Components::Attack>(card_ID)->range;

	std::string card_name = ecs.getComponent<Components::Name>(card_ID)->value;

	AEVec2 cur_part_pos = Get_CurPart_gridPos(ecs, tbs, gb);

	//iterate over range * 2 
	for (int ite = 1; ite <= card_range; ++ite)
	{
		if (cur_part_pos.x + ite < MAX_I) 
		{
			gb.get_highlighted_cell().push_back({ cur_part_pos.x + ite , cur_part_pos.y });
			gb.get_attack_activate()[cur_part_pos.x + ite][cur_part_pos.y] = true;
		}
		if (cur_part_pos.y + ite < MAX_J)
		{
			gb.get_highlighted_cell().push_back({ cur_part_pos.x , cur_part_pos.y + ite });
			gb.get_attack_activate()[cur_part_pos.x][cur_part_pos.y + ite] = true;
		}
		if (cur_part_pos.x - ite > -1) 
		{
			gb.get_highlighted_cell().push_back({ cur_part_pos.x - ite , cur_part_pos.y });
			gb.get_attack_activate()[cur_part_pos.x - ite][cur_part_pos.y] = true;
		}
		if (cur_part_pos.y - ite > -1) 
		{ 
			gb.get_highlighted_cell().push_back({ cur_part_pos.x , cur_part_pos.y - ite }); 
			gb.get_attack_activate()[cur_part_pos.x][cur_part_pos.y - ite] = true;
		}
	}
	//=========================================================================
}

AEVec2& Get_CurPart_gridPos(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb)
{
	Entity cur_part = tbs.current();
	std::array<std::array<Entity, MAX_J>, MAX_I>& positions = gb.get_pos();

	AEVec2 temp = { -1.f,-1.f };

	for (int i = 0; i < MAX_I; ++i)
	{
		for (int j = 0; j < MAX_J; ++j)
		{
			if (positions[i][j] == cur_part)
			{
				AEVec2Set(&temp, (f32)i, (f32)j);
				break;
			}
		}
		if (temp.x != -1.f && temp.y != -1.f) break;
	}

	return temp;
}