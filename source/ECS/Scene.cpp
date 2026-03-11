#include "Scene.h"

// STEVEN HERE IS THE HELPER - Zejin
Entity spawnEnemyAndBind(ECS::Registry& ecs,
	MeshFactory& mf,
	TextureFactory::TextureFactory& tf,
	EnemyDirector& enemyDirector,
	const std::string& actorId,
	const char* name,
	AEVec2 pos,
	Entity fa,
	Entity sa)
{
	Entity e = System::create_actor(ecs, mf, pos, { 192.0f, 192.0f }, name, 100.f, tf.getTextureChar(1));
	System::add_card_player(ecs, e, fa);
	System::add_card_player(ecs, e, sa);
	enemyDirector.bindActor(actorId, e);
	return e;
}

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
	temp = System::create_actor(*ecs, mf, { 0.f,0.f }, { 192.0f,192.0f }, "Player", 100.f, tf.getTextureChar(0));
	playerID = temp;//important must set the playerID !!!!!!!!!!!
	add_entity(temp);

	//Create Horde
	Entity horde = ecs->createEntity();
	ecs->addComponent(horde, Components::Name{ "Horde" });
	ecs->addComponent(horde, Components::TurnBasedStats{});

	enemyDirector.loadScriptFile("Assets/levels/TEST_level.txt"); //load enemy instrucitons

	//Holt shit the enemy script is so cool VVV

	////Add enemy0
	//temp = spawnEnemyAndBind(*ecs, mf, tf, enemyDirector, "E0", "Enemy0", { 100.f, 100.f }, fa, sa);
	//add_entity(temp);

	////Add enemy1
	//temp = spawnEnemyAndBind(*ecs, mf, tf, enemyDirector, "E1", "Enemy1", { 200.f, 100.f }, fa, sa);
	//add_entity(temp);

	for (int i = 0; i < enemyDirector.getSpawnCount(); ++i)
	{
		std::string actorId = "E" + std::to_string(i);
		std::string enemyName = "Enemy" + std::to_string(i);

		// temporary spawn position logic
		AEVec2 spawnPos = { 100.f + 100.f * i, 100.f };

		temp = System::create_actor(*ecs, mf, spawnPos, { 192.0f,192.0f }, enemyName.c_str(), 100.f, tf.getTextureChar(1));

		add_entity(temp);                  // adds to scene/world
		enemyDirector.bindActor(actorId, temp);
	}

	//New Vector for TBS
	std::vector<Entity> tbsParticipants;
	tbsParticipants.push_back(playerID);
	tbsParticipants.push_back(horde);

	TBSys.init(*ecs, eventPool, BattleGrid, gbs, card_system, ch, tbsParticipants);
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
	enemyDirector.update(*ecs, gbs, TBSys, BattleGrid, playerID);
	//==================Handle Events===============================

	if (eventPool.template_pool[HIGHLIGHT_EVENT].triggered)
	{
		highlight_tag highlight_type = eventPool.template_pool[HIGHLIGHT_EVENT].returned_value;

		switch (highlight_type)
		{
		case highlight_tag::ATTACK_HIGHLIGHT: 
		{
			Entity card_ID = TBSys.draw_card(*ecs, TBSys.current(), TBSys.get_selected_cardhand_index());
			f32& card_range = ecs->getComponent<Components::Attack>(card_ID)->range;
			highlight_cells(*ecs, TBSys, BattleGrid, card_range, highlight_type);

			break;
		}
		case highlight_tag::MOVE_HIGHLIGHT:
		{
			f32& range = ecs->getComponent<Components::TurnBasedStats>(TBSys.current())->cur_movSpd;
			highlight_cells(*ecs, TBSys, BattleGrid, range, highlight_type);
		}
			break;
		default:
			break;
		}

		eventPool.template_pool[HIGHLIGHT_EVENT].triggered = false;
	}

	if (eventPool.template_pool[UNHIGHLIGHT_EVENT].triggered)
	{
		unhighlight_cells(BattleGrid);
		eventPool.template_pool[UNHIGHLIGHT_EVENT].triggered = false;
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

void highlight_cells(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb,int range,highlight_tag type)
{
	//=========================Highlight_cells=================================
	AEVec2 cur_part_pos = Get_CurPart_gridPos(ecs, tbs, gb);

	for (int i = 0; i <= range; ++i)
	{
		for (int j = 0; j <= range; ++j)
		{
			if (i + j == 0) continue;
			if (i + j <= range && cur_part_pos.x + i < MAX_I && cur_part_pos.y + j < MAX_J)
			{
				if(gb.activate_highlight()[cur_part_pos.x + i][cur_part_pos.y + j] != highlight_tag::UNHIGHLIGHTED)
					continue;
				gb.get_highlighted_cell().push_back({ cur_part_pos.x + i , cur_part_pos.y + j });
				gb.activate_highlight()[cur_part_pos.x + i][cur_part_pos.y + j] = type;
			}
			if (i + j <= range &&cur_part_pos.x - i >= 0 && cur_part_pos.y - j >= 0)
			{
				if (gb.activate_highlight()[cur_part_pos.x - i][cur_part_pos.y - j] != highlight_tag::UNHIGHLIGHTED)
					continue;
				gb.get_highlighted_cell().push_back({ cur_part_pos.x - i , cur_part_pos.y - j });
				gb.activate_highlight()[cur_part_pos.x - i][cur_part_pos.y - j] = type;
			}
			if (i + j <= range && cur_part_pos.x + i < MAX_I && cur_part_pos.y - j >= 0)
			{
				if (gb.activate_highlight()[cur_part_pos.x + i][cur_part_pos.y - j] != highlight_tag::UNHIGHLIGHTED)
					continue;
				gb.get_highlighted_cell().push_back({ cur_part_pos.x + i , cur_part_pos.y - j });
				gb.activate_highlight()[cur_part_pos.x + i][cur_part_pos.y - j] = type;
			}
			if (i + j <= range && cur_part_pos.x - i >= 0 && cur_part_pos.y + j < MAX_J)
			{
				if (gb.activate_highlight()[cur_part_pos.x - i][cur_part_pos.y + j] != highlight_tag::UNHIGHLIGHTED)
					continue;
				gb.get_highlighted_cell().push_back({ cur_part_pos.x - i , cur_part_pos.y + j });
				gb.activate_highlight()[cur_part_pos.x - i][cur_part_pos.y + j] = type;
			}
		}
	}
	//=========================================================================
}

void unhighlight_cells(Grid::GameBoard& gb)
{
	{
		for (AEVec2 a : gb.get_highlighted_cell())
		{
			gb.activate_highlight()[int(a.x)][int(a.y)] = highlight_tag::UNHIGHLIGHTED;
		}
		gb.get_highlighted_cell().clear();
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