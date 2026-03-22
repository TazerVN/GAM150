#include "pch.h"
#include "Scene.h"
#include <ctime> // for randomiser -Zejin (FOR NOW...)
#include <cstdlib> // randomiser part 2
#include "../util/LevelManager.h"
#include "factory/EntityFactory.h"
#include "../UI/UI.h"

// STEVEN HERE IS THE HELPER - Zejin
Entity spawnEnemyAndBind(EntityComponent::Registry& ecs,
	EnemyDirector& enemyDirector,
	const std::string& actorId,
	const char* name,
	AEVec2 pos,
	Entity fa,
	Entity sa)
{
	Entity e = EntityFactory::create_actor_normal(ecs, mf, pos, { 192.0f, 192.0f }, name, 100.f, TF.getTextureChar(1), Components::AnimationType::IDLE);
	enemyDirector.bindActor(actorId, e);
	return e;
}




void Scene::init(Camera::CameraSystem& cam, UI::UIManager& _UI)

{
	std::srand(static_cast<unsigned>(std::time(nullptr)));
	cameraSys = &cam;
	UIptr = &_UI;

	s32 w_width = AEGfxGetWindowWidth();
	s32 w_height = AEGfxGetWindowHeight();

	//add player to the scene
	add_entity_to_scene(playerID);
	//Create Horde
	Entity horde = ecs.createEntity();
	ecs.addComponent(horde, Components::Name{ "Horde" });
	ecs.addComponent(horde, Components::TurnBasedStats{});
	ecs.addComponent(horde, Components::Horde_Tag{});
	
	Components::TurnBasedStats* st = ecs.getComponent<Components::TurnBasedStats>(playerID);
	st->max_movSpd = st->ini_movSpd;
	st->cur_movSpd = st->max_movSpd;

	enemyDirector.loadScriptFile("Assets/levels/TEST_level.txt"); //load enemy instrucitons


	for (int i = 0; i < enemyDirector.getSpawnCount(); ++i)
	{
		std::string actorId = "E" + std::to_string(i);
		std::string enemyName = "Enemy" + std::to_string(i);

		// temporary spawn position logic
		AEVec2 spawnPos = { 100.f + 100.f * i, 100.f };

		/*Entity temp = EntityFactory::create_actor_normal(ecs, mf, spawnPos, { 192.0f,192.0f }, enemyName.c_str(), 100.f, TF.getTextureChar(1), Components::AnimationType::IDLE);*/
		Entity temp = beastiary.generate_enemy_from_beastiary("Ranger", spawnPos, { 192.0f,192.0f }, Components::AnimationType::IDLE);

		add_entity_to_scene(temp);                  // adds to scene/world
		ecs.getComponent<Components::Horde_Tag>(horde)->goons.push_back(temp);
		enemyDirector.bindActor(actorId, temp);
	}

	cbs.init(ecs, gbs, BattleGrid ,TBSys, _UI.getCardHand(), eventPool);
	TBSys.init(ecs,eventPool, BattleGrid, gbs, cbs, card_system, _UI.getCardHand(), horde);
	BattleGrid.init(&TBSys, eventPool, gbs, cbs, TF.getTextureFloor(0), 0, w_height / 3,_win);

	gbs.resetGPhase();
	gbs.resetPlayerPhase();

	//nameTags.create_static_nametag(playerID, "Player");

	//place entitities
	for (size_t i = 0; i < entities.size(); ++i)
	{
		s32 x = 0;
		s32 y = 0;

		const int maxAttempts = 100;
		int attempts = 0;

		do
		{
			x = std::rand() % MAX_I;
			y = std::rand() % MAX_J;
			++attempts;
		} while (BattleGrid.get_pos()[x][y] != Components::NULL_INDEX && attempts < maxAttempts);

		if (BattleGrid.get_pos()[x][y] == Components::NULL_INDEX)
		{
			BattleGrid.placeEntity(entities[i], x, y);
		}
		else
		{
			std::cout << "[Scene] Failed to place entity " << entities[i] << " on empty tile.\n";
		}
	}


}

void Scene::update()
{
	if (AEInputCheckTriggered(AEVK_F1))
	{
		BattleGrid.in_walkable_debug = !BattleGrid.in_walkable_debug;
	}

	if (AEInputCheckTriggered(AEVK_F2))
	{
		BattleGrid.in_pos_debug = !BattleGrid.in_pos_debug;
	}

	if (AEInputCheckTriggered(AEVK_BACKSLASH))
	{
		BattleGrid.debug_print();
	}
	if (AEInputCheckTriggered(AEVK_RSHIFT))
	{
		TBSys.debug_print(ecs);
	}
	if (AEInputCheckTriggered(AEVK_EQUAL))
	{
		/*for (Entity card : ecs.getComponent<Components::Card_Storage>(playerID)->original_draw_pile)
		{
			
		}*/
		std::cout << "Original Deck size : " << ecs.getComponent<Components::Card_Storage>(playerID)->original_draw_pile.size() 
			<< std::endl;
	}
	if (AEInputCheckTriggered(AEVK_UP))
	{
		gbs.debug_print();
	}
	//nameTags.update();
	if (TBSys.active())
	{
		if (TBSys.update())
		{
			std::cout << "WIN!!!!" << std::endl;
			TBSys.active() = false;
			_win = true;

			ecs.getComponent<Components::TurnBasedStats>(playerID)->cur_movSpd = 100.f;
			ecs.getComponent<Components::TurnBasedStats>(playerID)->max_movSpd = 100.f;
			UIptr->getVictoryMenu().on = true;

			Entity BossNode;
			BossNode = iNodes.create_interactable_node(ecs, mf, { 0.0f,0.f }, { 192.0f,192.0f }, TF.getTextureOthers(1),
				Components::AnimationType::NONE, Components::VictoryNodeTag::BOSS);

			Entity combatNode;
			combatNode = iNodes.create_interactable_node(ecs, mf, { 0.0f,0.f }, { 192.0f,192.0f }, TF.getTextureOthers(2),
				Components::AnimationType::NONE, Components::VictoryNodeTag::COMBAT);
			BattleGrid.placeEntity(combatNode, 0, 0);
			BattleGrid.placeEntity(BossNode, MAX_I - 1, MAX_J - 1);
		}

		cbs.update();
		enemyDirector.update(gbs, TBSys, BattleGrid);
	}

	//==================Handle Events===============================

	if (eventPool.template_pool[UNHIGHLIGHT_EVENT].triggered)
	{
		unhighlight_cells(BattleGrid, cbs);
		eventPool.template_pool[UNHIGHLIGHT_EVENT].triggered = false;
	}

	if (eventPool.template_pool[HIGHLIGHT_EVENT].triggered)
	{
		highlight_tag highlight_type = eventPool.template_pool[HIGHLIGHT_EVENT].returned_value;

		switch (highlight_type)
		{
		case highlight_tag::ATTACK_HIGHLIGHT:
		{
			Entity card_ID = TBSys.draw_card(TBSys.current(), TBSys.get_selected_cardhand_index());
			f32& card_range = ecs.getComponent<Components::Targetting_Component>(card_ID)->range;

			Targetting targetting = ecs.getComponent<Components::Targetting_Component>(card_ID)->targetting_type;
			highlight_cells(targetting == Targetting::LINE,TBSys, BattleGrid, cbs, card_range, highlight_type);

			break;
		}
		case highlight_tag::MOVE_HIGHLIGHT:
		{
			f32& range = ecs.getComponent<Components::TurnBasedStats>(TBSys.current())->cur_movSpd;
			highlight_cells(false,TBSys, BattleGrid, cbs, range, highlight_type);
		}
		break;
		default:
			break;
		}

		eventPool.template_pool[HIGHLIGHT_EVENT].triggered = false;
	}
	//==============================================================
}

void Scene::add_entity_to_scene(Entity e)
{
	next_entity++;
	entities.push_back(e);
}

std::vector<Entity>& Scene::entities_store()
{
	return entities;
}

void Scene::scene_free()
{
	TBSys.tbs_free();
	BattleGrid.gameboard_free();
	cameraSys = nullptr;
	entities.clear();
	next_entity = 0;
	_win = false;
	//nameTags.name_tag_free();
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

CombatNameSpace::CombatSystem& Scene::getCombatSystem()
{
	return cbs;
}

bool Scene::win()const
{
	return _win;
}
void highlight_cells(bool line,TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb, CombatNameSpace::CombatSystem& cbs ,int range,highlight_tag type)
{
	//=========================Highlight_cells=================================

	AEVec2 cur_part_pos = gb.Get_CurPart_gridPos();

	if (line)
	{
		for (int itr = 0; itr <= range; ++itr)
		{
			if (cur_part_pos.x + itr < MAX_I)
			{
				cbs.get_highlighted_cell().push_back({ cur_part_pos.x + itr, cur_part_pos.y });
				gb.activate_highlight()[cur_part_pos.x + itr][cur_part_pos.y] = type;
			}
			if (cur_part_pos.x - itr >= 0)
			{
				cbs.get_highlighted_cell().push_back({ cur_part_pos.x - itr, cur_part_pos.y });
				gb.activate_highlight()[cur_part_pos.x - itr][cur_part_pos.y] = type;
			}
			if (cur_part_pos.y + itr < MAX_J)
			{
				cbs.get_highlighted_cell().push_back({ cur_part_pos.x, cur_part_pos.y + itr });
				gb.activate_highlight()[cur_part_pos.x][cur_part_pos.y + itr] = type;
			}
			if (cur_part_pos.y - itr >= 0)
			{
				cbs.get_highlighted_cell().push_back({ cur_part_pos.x, cur_part_pos.y - itr });
				gb.activate_highlight()[cur_part_pos.x][cur_part_pos.y - itr] = type;
			}


		}
	}
	else 
	{
		for (int i = 0; i <= range; ++i)
		{
			for (int j = 0; j <= range; ++j)
			{
				//if (i + j == 0) continue;
				if (i + j <= range && cur_part_pos.x + i < MAX_I && cur_part_pos.y + j < MAX_J)
				{
					if (gb.activate_highlight()[cur_part_pos.x + i][cur_part_pos.y + j] != highlight_tag::UNHIGHLIGHTED)
						continue;
					cbs.get_highlighted_cell().push_back({ cur_part_pos.x + i , cur_part_pos.y + j });
					gb.activate_highlight()[cur_part_pos.x + i][cur_part_pos.y + j] = type;
				}
				if (i + j <= range && cur_part_pos.x - i >= 0 && cur_part_pos.y - j >= 0)
				{
					if (gb.activate_highlight()[cur_part_pos.x - i][cur_part_pos.y - j] != highlight_tag::UNHIGHLIGHTED)
						continue;
					cbs.get_highlighted_cell().push_back({ cur_part_pos.x - i , cur_part_pos.y - j });
					gb.activate_highlight()[cur_part_pos.x - i][cur_part_pos.y - j] = type;
				}
				if (i + j <= range && cur_part_pos.x + i < MAX_I && cur_part_pos.y - j >= 0)
				{
					if (gb.activate_highlight()[cur_part_pos.x + i][cur_part_pos.y - j] != highlight_tag::UNHIGHLIGHTED)
						continue;
					cbs.get_highlighted_cell().push_back({ cur_part_pos.x + i , cur_part_pos.y - j });
					gb.activate_highlight()[cur_part_pos.x + i][cur_part_pos.y - j] = type;
				}
				if (i + j <= range && cur_part_pos.x - i >= 0 && cur_part_pos.y + j < MAX_J)
				{
					if (gb.activate_highlight()[cur_part_pos.x - i][cur_part_pos.y + j] != highlight_tag::UNHIGHLIGHTED)
						continue;
					cbs.get_highlighted_cell().push_back({ cur_part_pos.x - i , cur_part_pos.y + j });
					gb.activate_highlight()[cur_part_pos.x - i][cur_part_pos.y + j] = type;
				}
			}
		}
	}
	//=========================================================================
}

void unhighlight_cells(Grid::GameBoard& gb,CombatNameSpace::CombatSystem& cbs)
{
	for (AEVec2 a : cbs.get_highlighted_cell())
	{
		gb.activate_highlight()[int(a.x)][int(a.y)] = highlight_tag::UNHIGHLIGHTED;
	}
	cbs.get_highlighted_cell().clear();
}