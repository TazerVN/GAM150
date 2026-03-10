#include "Scene.h"
#include "../factory/EntityFactory.h"

void Scene::init(ECS::Registry& ECS,MeshFactory& mf, CardSystem& cs, TextureFactory::TextureFactory& tf, Camera::CameraSystem& cam, CardInteraction::CardHand& ch)
{
	cameraSys = &cam;
	cardSys = &cs;

	s32 w_width = AEGfxGetWindowWidth();
	s32 w_height = AEGfxGetWindowHeight();

	//must init appoint ecs first
	ecs = &ECS;
	Entity sa = cardSys->get_card(CardSystemNames::SLASH);
	Entity fa = cardSys->get_card(CardSystemNames::SLASH_PLUS);
	Entity ss = cardSys->get_card(CardSystemNames::SHOOT);
	Entity blackHole = cardSys->get_card(CardSystemNames::SHOOT_PLUS);
	//add cards to the player
	Entity temp; 
	
	//Add player
	temp = System::create_actor_spritesheet(*ecs, mf, { 0.f,0.f }, { 192.0f,192.0f }, "Player", 100.f, tf.getTextureChar(2));
	playerID = temp;//important must set the playerID !!!!!!!!!!!
	add_entity(temp);

	/*for (int i = 0; i < 8; ++i)
	{
		System::add_card_player_deck(*ecs, temp, sa);
		System::add_card_player_deck(*ecs, temp, fa);
	}
	System::add_card_player_deck(*ecs, temp, ss);*/
	/*for (int i = 0; i < 8; ++i)
	{
		System::add_card_player_deck(*ecs, temp, blackHole);
	}*/
	System::add_card_player_deck(*ecs, temp, blackHole);
	System::add_card_player_deck(*ecs, temp, sa);
	
	enemyDirector.loadScriptFile("Assets/levels/TEST_level.txt"); //load enemy instrucitons

	//Holt shit the enemy script is so cool VVV

	//Add enemy0
	temp = System::create_actor_normal(*ecs, mf, { 100.f,100.f }, { 192.0f,192.0f }, "Enemy0", 100.f, tf.getTextureChar(0));
	System::add_card_player_hand(*ecs, temp, fa);	//add fire attack
	System::add_card_player_hand(*ecs, temp, sa);	//add sword attack
	add_entity(temp);
	enemyDirector.bindActor("E0", temp);		// enemy now bound as E0

	//Add enemy1
	temp = System::create_actor_normal(*ecs, mf, { 100.f,100.f }, { 192.0f,192.0f }, "Enemy", 100.f, tf.getTextureChar(1));
	System::add_card_player_hand(*ecs, temp, fa);	//add fire attack
	System::add_card_player_hand(*ecs, temp, sa);	//add sword attack
	add_entity(temp);
	enemyDirector.bindActor("E1", temp);		// enemy now bound as E1

	TBSys.init(*ecs,eventPool, BattleGrid, gbs, *cardSys, ch ,entities);
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
			f32& card_range = ecs->getComponent<Components::Targetting_Component>(card_ID)->range;

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

	AEVec2 cur_part_pos = gb.Get_CurPart_gridPos();

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
	for (AEVec2 a : gb.get_highlighted_cell())
	{
		gb.activate_highlight()[int(a.x)][int(a.y)] = highlight_tag::UNHIGHLIGHTED;
	}
	gb.get_highlighted_cell().clear();
}