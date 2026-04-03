#include "pch.h"
#include "Scene.h"
#include <ctime> // for randomiser -Zejin (FOR NOW...)
#include <cstdlib> // randomiser part 2
#include "../util/LevelManager.h"
#include "factory/EntityFactory.h"
#include "../UI/UI.h"
//#include "util/util.h"

std::vector<std::string> hardcoded_levels;

// STEVEN HERE IS THE HELPER - Zejin
Entity spawnEnemyAndBind(EnemyDirector& enemyDirector,
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
	unsigned int seed;

	if (parse_seed(seed, "Assets/levels/cur_seed.json") != JSON_RET::OK)
	{
		seed = static_cast<unsigned int>(time(nullptr));
	}

	std::srand(seed);

	cameraSys = &cam;
	UIptr = &_UI;
	iNodes.init(BattleGrid,gbs);

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

	std::vector<Entity> rocks;

	if (tutorial_active)
	{
		std::cout << "[Tutorial] Movement stage init\n";

		// Reset player stamina
		Components::TurnBasedStats* st = ecs.getComponent<Components::TurnBasedStats>(playerID);
		st->max_movSpd = 5.f;   // small for teaching
		st->cur_movSpd = 5.f;

		tutorial_stage = TutorialStage::MOVEMENT;
		tutorial_substep = 0;
		tutorial_goal_reached = false;

		tutorial_goal_x = (MAX_I / 2) + 3;
		tutorial_goal_y = MAX_J / 2;

		std::cout << "Select player, then click this tile to move.\n";
	}
	else
	{

		//enemyDirector.loadScriptFile("Assets/levels/TEST_level.txt"); //load enemy instrucitons
		if (SS.firstLevel())
		{
			enemyDirector.loadScriptFile("Assets/levels/BEGINNER_COMBAT.txt");//load enemy instrucitons
			//enemyDirector.loadScriptFile("Assets/levels/FistFight.txt");//load enemy instrucitons
		}
		else
		{
			hardcoded_levels.reserve(sizeof(std::string) * 3);
			hardcoded_levels.push_back("Clutter");
			hardcoded_levels.push_back("TEST_level");
			hardcoded_levels.push_back("TrollAllEnemy");
			hardcoded_levels.push_back("ONESHOTWONDER");
			hardcoded_levels.push_back("FistFight");

			int upper_bound = static_cast<int>(hardcoded_levels.size()) - 1;
			int lower_bound = 0;
			int index = std::rand() % (upper_bound - lower_bound + 1) + lower_bound;

			std::string loc = "Assets/levels/";
			std::string end = ".txt";
			loc += hardcoded_levels[index] += end;

			enemyDirector.loadScriptFile(loc);//load enemy instrucitons

			hardcoded_levels.clear();
		}
	}

	cbs.init(gbs, BattleGrid, TBSys, _UI.getCardHand(), eventPool, highlightSystem, intentDisplaySystem, enemyDirector);
	TBSys.init(eventPool, BattleGrid, gbs, cbs, card_system, _UI.getCardHand(), horde);
	BattleGrid.init(&TBSys, eventPool, gbs, cbs, highlightSystem, TF.getTextureFloor(0), 0, w_height / 3, _win);
	gbs.resetGPhase();
	gbs.resetPlayerPhase();

	if (!tutorial_active)
	{
		for (int i = 0; i < enemyDirector.getSpawnCount(); ++i)
		{
			std::string actorId = "E" + std::to_string(i);
			std::string enemyName = "Enemy" + std::to_string(i);

			AEVec2 spawnPos = { 100.f + 100.f * i, 100.f };

			const int totalEnemies = enemyDirector.getSpawnCount();
			const int rangedEnemies = enemyDirector.getRangedSpawnCount();

			bool isRanged = (i >= totalEnemies - rangedEnemies);
			const std::string beastKey = isRanged ? "Ranger" : "Melee";

			Entity temp = beastiary.generate_enemy_from_beastiary(
				beastKey,
				spawnPos,
				{ 192.0f,192.0f },
				Components::AnimationType::IDLE
			);

			add_entity_to_scene(temp);
			ecs.getComponent<Components::Horde_Tag>(horde)->goons.push_back(temp);
			enemyDirector.bindActor(actorId, temp);
		}

		for (int i = 0; i < enemyDirector.getMapObjectCount(); ++i)
		{
			Entity rock = EntityFactory::create_grid_object(
				ecs,
				mf,
				{ 0.0f, 0.0f },
				{ 298.0f, 258.0f },
				"Rock",
				TF.getTextureOthers(0),
				1.0f,
				1
			);

			Components::Color* rockColor = ecs.getComponent<Components::Color>(rock);
			if (rockColor)
			{
				rockColor->c_color.r = 0.5f;
				rockColor->c_color.g = 0.7f;
				rockColor->c_color.b = 1.0f;
				rockColor->d_color = rockColor->c_color;
			}

			rocks.push_back(rock);
		}
	}


	if (tutorial_active)
	{
		// ===== TUTORIAL PLACEMENT =====
		s32 centerX = MAX_I / 2;
		s32 centerY = MAX_J / 2;

		BattleGrid.placeEntity(playerID, centerX, centerY);
	}
	else
	{
		// ===== NORMAL ENTITY PLACEMENT =====
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
		}

		for (Entity rock : rocks)
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
				BattleGrid.placeEntity(rock, x, y);
			}
		}
	}

	intentDisplaySystem.init(enemyDirector,highlightSystem,BattleGrid);

	intentDisplaySystem.trigger();
}

void Scene::update()
{
	if (tutorial_active)
	{
		update_tutorial();
	}

	if (ConsoleEvents.template_pool[static_cast<size_t>(CommandTypes::CT_KILL_ENEMIES)].triggered || AEInputCheckTriggered(AEVK_MINUS))
	{
		ConsoleEvents.template_pool[static_cast<size_t>(CommandTypes::CT_KILL_ENEMIES)].triggered = false;
		for (Entity goon : ecs.getComponent<Components::Horde_Tag>(TBSys.get_participant()[1])->goons)
		{
			AEVec2 pos = { f32(ecs.getComponent<Components::gridData>(goon)->x),
						   f32(ecs.getComponent<Components::gridData>(goon)->y) };
			cbs.get_graveyard().push_back({ pos,goon });
		}
	}

	if (ConsoleEvents.template_pool[static_cast<size_t>(CommandTypes::CT_HIGHLIGHT_WALKABLE)].triggered)
	{
		ConsoleEvents.template_pool[static_cast<size_t>(CommandTypes::CT_HIGHLIGHT_WALKABLE)].triggered = false;
		BattleGrid.in_walkable_debug = !BattleGrid.in_walkable_debug;
	}

	if (ConsoleEvents.template_pool[static_cast<size_t>(CommandTypes::CT_HIGHLIGHT_POSITION)].triggered)
	{
		ConsoleEvents.template_pool[static_cast<size_t>(CommandTypes::CT_HIGHLIGHT_POSITION)].triggered = false;
		BattleGrid.in_pos_debug = !BattleGrid.in_pos_debug;
	}

	if (AEInputCheckTriggered(AEVK_RCTRL)) // test particle
	{
		f32 x; f32 width = 40.f;
		f32 y; f32 height = 40.f;
		translate_To_Isometric(BattleGrid.GetOffsetPos(), height, x, y, BattleGrid.cur_x, BattleGrid.cur_y);
		PS.particleShield(x,y + 50.f,0.2f,0.f,1.f,1.f,2.f,50);
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
			<< '\n';
	}
	if (AEInputCheckTriggered(AEVK_UP))
	{
		gbs.debug_print();
	}
	if (AEInputCheckTriggered(AEVK_DOWN))
	{
		enemyDirector.print_current_instruction();
	}
	//nameTags.update();
	if (!tutorial_active && TBSys.active())
	{
		
			if (TBSys.update())
			{
				std::cout << "WIN!!!!" << '\n';
				TBSys.active() = false;
				_win = true;

				ecs.getComponent<Components::TurnBasedStats>(playerID)->cur_movSpd = 100.f;
				ecs.getComponent<Components::TurnBasedStats>(playerID)->max_movSpd = 100.f;
				UIptr->getVictoryMenu().on = true;

				Entity firstNode;
				firstNode = iNodes.create_interactable_node(ecs, mf, { 0.0f,0.f }, { 256.f,256.f }, TF.getTextureOthers(1),
					Components::AnimationType::NONE, Components::VictoryNodeTag::COMBAT);

				Entity secondNode;
				secondNode = iNodes.create_interactable_node(ecs, mf, { 0.0f,0.f }, { 256.f,256.f }, TF.getTextureOthers(1),
					Components::AnimationType::NONE, Components::VictoryNodeTag::COMBAT);

				unsigned int seed = static_cast<unsigned int>(time(nullptr));
				if (parse_date_to_file(seed, "Assets/levels/cur_seed.json") != JSON_RET::OK)
				{
					std::cout << " put data fail";
				}

				BattleGrid.placeEntity(firstNode, 0, 0);
				BattleGrid.placeEntity(secondNode, MAX_I - 1, MAX_J - 1);

				SS.incrementLevelCleared();
			}
		
	}
	
	if (!tutorial_active)
	{
		enemyDirector.update(gbs, TBSys, BattleGrid, intentDisplaySystem);
	}

	cbs.update();
	intentDisplaySystem.update(*this);
	iNodes.update();
	//==================Handle Events===============================

	highlightSystem.update();
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
	UIptr = nullptr;
	entities.clear();
	next_entity = 0;
	PS.particle_system_free();
	intentDisplaySystem.intentionSystem_free();
	_win = false;
	gbs.gbs_free();
	EntityFactory::free_Player();
	
	playerBarrier = Components::NULL_INDEX; // shield display
	cbs.combatSystem_free();
}

void Scene::advance_tutorial_stage(TutorialStage nextStage)
{
	tutorial_stage = nextStage;
	tutorial_substep = 0;
	tutorial_goal_reached = false;
	tutorial_goal_x = 0;
	tutorial_goal_y = 0;
}

void Scene::update_tutorial()
{
	switch (tutorial_stage)
	{
	case TutorialStage::MOVEMENT:
		update_tutorial_movement();
		break;

	case TutorialStage::ATTACK_CARD:
		// later
		break;

	case TutorialStage::DEFENSE_CARD:
		// later
		break;

	case TutorialStage::ITEM_CARD:
		// later
		break;

	case TutorialStage::EVENT_CARD:
		// later
		break;

	case TutorialStage::WIN_TRANSITION:
		// later
		break;

	case TutorialStage::DONE:
		break;
	}
}

void Scene::set_tutorial_active(bool active)
{
	tutorial_active = active;
}

bool Scene::is_tutorial_active() const
{
	return tutorial_active;
}

void Scene::update_tutorial_movement()
{
	Components::gridData* gd = ecs.getComponent<Components::gridData>(playerID);
	if (!gd)
		return;

	switch (tutorial_substep)
	{
	case 0:
	{
		// Show this in UI later:
		// "Select player, then click this tile to move."

		if (!tutorial_goal_reached &&
			gd->x == tutorial_goal_x &&
			gd->y == tutorial_goal_y)
		{
			tutorial_goal_reached = true;
			tutorial_substep = 1;
			std::cout << "Stamina bar determines how far you can move. Press Space to continue.\n";
		}
		break;
	}

	case 1:
	{
		if (AEInputCheckTriggered(AEVK_SPACE))
		{
			std::cout << "[Tutorial] Movement stage complete.\n";
			advance_tutorial_stage(TutorialStage::ATTACK_CARD);
		}
		break;
	}
	}
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