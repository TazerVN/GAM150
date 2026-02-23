#include "Scene.h"

void Scene::init(MeshFactory& mf, TextureFactory::TextureFactory& tf)
{
	s32 w_width = AEGfxGetWindowWidth();
	s32 w_height = AEGfxGetWindowHeight();

	card_system.init_cards(ecs);
	Entity sa = card_system.get_card(0);
	Entity fa = card_system.get_card(1);
	Entity ss = card_system.get_card(2);
	//add cards to the player
	Entity temp; 
	
	//Add player
	temp = System::create_player(ecs, mf, { 0.f,0.f }, { 192.0f,192.0f }, "Player1", 100.f, tf.getTexture(2));
	System::add_card_player(ecs, temp, sa);	//add sword attack
	System::add_card_player(ecs, temp, ss);	//add silver slash attack
	System::add_card_player(ecs, temp, fa);	//add sword attack

	playerID = temp;//important must set the playerID !!!!!!!!!!!

	add_entity(temp);

	//Add enemy0
	temp = System::create_player(ecs, mf, { 100.f,100.f }, { 192.0f,192.0f }, "Enemy0", 100.f, tf.getTexture(3));
	System::add_card_player(ecs, temp, fa);	//add fire attack
	System::add_card_player(ecs, temp, sa);	//add sword attack
	add_entity(temp);

	TBSys.init(eventPool);
	BattleGrid.init(ecs, mf, &TBSys, eventPool, tf.getTexture(1), 0, w_height / 3);
	
	//place entitities
	for (size_t i = 0; i < entities.size(); ++i)
	{
		BattleGrid.placeEntity(ecs, entities[i], 5 + i * 3, 5);
	}
}

void Scene::update()
{
	TBSys.update(ecs, entities);

	//==================Handle Events===============================
	if (eventPool.pool[HIGHLIGHT_EVENT].triggered)
	{
		highlight_cells(ecs, TBSys, BattleGrid);
		eventPool.pool[HIGHLIGHT_EVENT].triggered = false;
	}

	if (eventPool.pool[UNHIGHLIGHT_EVENT].triggered)
	{
		unhighlight_cells(BattleGrid);
		eventPool.pool[UNHIGHLIGHT_EVENT].triggered = false;
	}

	if (eventPool.pool[ATTACK_EVENT].triggered)
	{
		if (eventPool.pool[ATTACK_EVENT].returned_value == NULL_INDEX) return;
		Entity current_entt = TBSys.current();
		Entity cardID = TBSys.draw_card(ecs, current_entt, TBSys.get_selected_cardhand_index());
		TBSys.play_card(ecs, eventPool.pool[ATTACK_EVENT].returned_value, cardID);
		TBSys.set_selected_card(false);
		TBSys.next(ecs);

		eventPool.pool[ATTACK_EVENT].triggered = false;
		eventPool.pool[ATTACK_EVENT].returned_value = NULL_INDEX;
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
PhaseSystem::GameBoardState& Scene::getGBS(){
	return gbs;
}

ECS::Registry& Scene::getECS()
{
	return ecs;
}

Entity& Scene::get_playerID()
{
	return playerID;
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

void unhighlight_cells(Grid::GameBoard& gb)
{
	{
		//un-highligh cells
		for (AEVec2 a : gb.get_highlighted_cell())
		{
			gb.get_attack_activate()[a.x][a.y] = false;
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