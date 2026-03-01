#include "Scene.h"

void Scene::init(ECS::Registry& ECS,MeshFactory& mf, TextureFactory::TextureFactory& tf, CardInteraction::CardHand& ch)
{
	s32 w_width = AEGfxGetWindowWidth();
	s32 w_height = AEGfxGetWindowHeight();

	//must init appoint ecs first
	ecs = &ECS;

	card_system.init_cards(*ecs);
	Entity sa = card_system.get_card(0);
	Entity fa = card_system.get_card(1);
	Entity ss = card_system.get_card(2);
	//add cards to the player
	Entity temp; 
	
	//Add player
	temp = System::create_player(*ecs, mf, { 0.f,0.f }, { 192.0f,192.0f }, "Player1", 100.f, tf.getTextureChar(0));
	System::add_card_player(*ecs, temp, sa);	//add sword attack
	System::add_card_player(*ecs, temp, ss);	//add silver slash attack
	System::add_card_player(*ecs, temp, fa);	//add fa attack

	playerID = temp;//important must set the playerID !!!!!!!!!!!

	add_entity(temp);

	//Add enemy0
	temp = System::create_player(*ecs, mf, { 100.f,100.f }, { 192.0f,192.0f }, "Enemy0", 100.f, tf.getTextureChar(1));
	System::add_card_player(*ecs, temp, fa);	//add fire attack
	System::add_card_player(*ecs, temp, sa);	//add sword attack
	add_entity(temp);

	//Add enemy1
	temp = System::create_player(*ecs, mf, { 100.f,100.f }, { 192.0f,192.0f }, "Enemy", 100.f, tf.getTextureChar(1));
	System::add_card_player(*ecs, temp, fa);	//add fire attack
	System::add_card_player(*ecs, temp, sa);	//add sword attack
	add_entity(temp);

	TBSys.init(*ecs,eventPool, BattleGrid, gbs, card_system, ch ,entities);
	BattleGrid.init(*ecs, mf, &TBSys, eventPool, gbs, tf.getTextureFloor(0), 0, w_height / 3);
	
	//place entitities
	for (size_t i = 0; i < entities.size(); ++i)
	{
		s32 x = s32(AERandFloat() * f32(MAX_I));
		s32 y = s32(AERandFloat() * f32(MAX_J));
		BattleGrid.placeEntity(*ecs, entities[i], x, y);
	}


}

void Scene::update()
{
	TBSys.update(*ecs);
	//==================Handle Events===============================

	if (eventPool.pool[HIGHLIGHT_EVENT].triggered)
	{
		highlight_cells(*ecs, TBSys, BattleGrid);
		eventPool.pool[HIGHLIGHT_EVENT].triggered = false;
	}

	//if (eventPool.pool[PLAY_CARD_EVENT].triggered)
	//{
	//	if (eventPool.pool[PLAY_CARD_EVENT].returned_value == NULL_INDEX) return;

	//	//determine target 
	//	Entity target = eventPool.pool[PLAY_CARD_EVENT].returned_value;
	//	Entity current_entt = TBSys.current();
	//	Entity cardID = TBSys.draw_card(ecs, current_entt, TBSys.get_selected_cardhand_index());
	//	bool died = TBSys.play_card(ecs, target, cardID);
	//	if (died)
	//	{
	//		int x = eventPool.pool[PLAY_CARD_EVENT].x;
	//		int y = eventPool.pool[PLAY_CARD_EVENT].y;

	//		if (x != -1 && y != -1)
	//			BattleGrid.get_pos()[x][y] = -1;
	//		TBSys.remove_participant(ecs, target);
	//	}
	//	eventPool.pool[PLAY_CARD_EVENT].triggered = false;
	//	eventPool.pool[PLAY_CARD_EVENT].returned_value = NULL_INDEX;
	//}

	if (eventPool.pool[UNHIGHLIGHT_EVENT].triggered)
	{
		unhighlight_cells(BattleGrid);
		eventPool.pool[UNHIGHLIGHT_EVENT].triggered = false;
	}

	//==============================================================
}

void Scene::add_entity(Entity e)
{
	next_entity++;
	entities.push_back(e);
}

std::vector<Entity>& Scene::entities_store()
{
	return entities;
}

ECS::Registry& Scene::getECS()
{
	return *ecs;
}

PhaseSystem::GameBoardState& Scene::getGBS(){
	return gbs;
}

TBS::TurnBasedSystem& Scene::getTBS()
{
	return TBSys;
}

Grid::GameBoard& Scene::getBattleGrid()
{
	return BattleGrid;
}

void highlight_cells(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb)
{
	//=========================Highlight_cells=================================
	Entity card_ID = tbs.draw_card(ecs, tbs.current(), tbs.get_selected_cardhand_index());
	f32& card_range = ecs.getComponent<Components::Attack>(card_ID)->range;

	const char* card_name = ecs.getComponent<Components::Name>(card_ID)->value;

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

void unhighlight_cells(Grid::GameBoard& gb)
{
	{
		//un-highligh cells
		for (AEVec2 a : gb.get_highlighted_cell())
		{
			gb.get_attack_activate()[int(a.x)][int(a.y)] = false;
			gb.get_highlighted_cell().clear();
		}
	}
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