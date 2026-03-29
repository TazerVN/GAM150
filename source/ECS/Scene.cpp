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

	//enemyDirector.loadScriptFile("Assets/levels/TEST_level.txt"); //load enemy instrucitons
	if (SS.firstLevel())
	{
		enemyDirector.loadScriptFile("Assets/levels/BEGINNER_COMBAT.txt");//load enemy instrucitons
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

	std::vector<Entity> rocks;

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




	cbs.init(gbs, BattleGrid ,TBSys, _UI.getCardHand(), eventPool,highlightSystem,intentDisplaySystem);
	TBSys.init(eventPool, BattleGrid, gbs, cbs, card_system, _UI.getCardHand(), horde);
	BattleGrid.init(&TBSys, eventPool, gbs, cbs, highlightSystem ,TF.getTextureFloor(0), 0, w_height / 3,_win);

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

			/*Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(rock);
			if (mesh)
			{
				mesh->z = 0;
			}*/
		}
		else
		{
			std::cout << "[Scene] Failed to place rock " << rock << " on empty tile.\n";
		}
	}

	gbs.resetGPhase();
	gbs.resetPlayerPhase();

	//Shield display - steven canu draw shield here
	/*this->playerBarrier = EntityFactory::create_grid_object(
		ecs,
		mf,
		{ 0.0f, 0.0f },
		{ 192.0f, 192.0f },
		"Barrier",
		TF.getTextureOthers(5),
		1.0f
	);

	Components::Mesh* barrierMesh = ecs.getComponent<Components::Mesh>(this->playerBarrier);
	if (barrierMesh)
	{
		barrierMesh->on = false;
	}*/

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

			Components::Tag* tag = ecs.getComponent<Components::Tag>(entities[i]);
			Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(entities[i]);

			//if (tag && mesh && *tag == Components::Tag::OTHERS)
			//{
			//	mesh->z = 0; // keep rocks under most other visuals
			//}
		}
		else
		{
			std::cout << "[Scene] Failed to place entity " << entities[i] << " on empty tile.\n";
		}
	}

	intentDisplaySystem.init(enemyDirector,highlightSystem,BattleGrid);

	intentDisplaySystem.trigger();
}

void Scene::update()
{
	if (AEInputCheckTriggered(AEVK_MINUS))
	{
		/*for (auto it : enemyDirector.get_map())
		{
			AEVec2 pos = { f32(ecs.getComponent<Components::gridData>(it.second)->x),
						   f32(ecs.getComponent<Components::gridData>(it.second)->y) };
			cbs.get_graveyard().push_back({pos,it.second});
		}*/
		for (Entity goon : ecs.getComponent<Components::Horde_Tag>(TBSys.get_participant()[1])->goons)
		{
			AEVec2 pos = { f32(ecs.getComponent<Components::gridData>(goon)->x),
						   f32(ecs.getComponent<Components::gridData>(goon)->y) };
			cbs.get_graveyard().push_back({ pos,goon });
		}
	}

	if (AEInputCheckTriggered(AEVK_RCTRL)) // test particle
	{
		f32 x; f32 width = 40.f;
		f32 y; f32 height = 40.f;
		translate_To_Isometric(BattleGrid.GetOffsetPos(), height, x, y, BattleGrid.cur_x, BattleGrid.cur_y);

		PS.particleShield(x,y + 50.f,0.2f,0.f,1.f,1.f,2.f,50);
	}

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
	if (AEInputCheckTriggered(AEVK_DOWN))
	{
		enemyDirector.print_current_instruction();
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

			Entity firstNode;
			firstNode = iNodes.create_interactable_node(ecs, mf, { 0.0f,0.f }, { 192.0f,192.0f }, TF.getTextureOthers(1),
				Components::AnimationType::NONE, Components::VictoryNodeTag::COMBAT);

			Entity secondNode;
			secondNode = iNodes.create_interactable_node(ecs, mf, { 0.0f,0.f }, { 192.0f,192.0f }, TF.getTextureOthers(2),
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
	cbs.update();
	enemyDirector.update(gbs, TBSys, BattleGrid, intentDisplaySystem);
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
	
	Entity playerBarrier = Components::NULL_INDEX; // shield display
	cbs.combatSystem_free();
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