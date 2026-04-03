#include "pch.h"
#include "Scene.h"
#include <ctime> // for randomiser -Zejin (FOR NOW...)
#include <cstdlib> // randomiser part 2
#include "../util/LevelManager.h"
#include "factory/EntityFactory.h"
#include "../UI/UI.h"
//#include "util/util.h"

std::vector<std::string> hardcoded_levels;

// Tutorial helper

static void clear_player_cards_for_tutorial(Entity owner, UI::UIManager* UIptr)
{
	Components::Card_Storage* storage = ecs.getComponent<Components::Card_Storage>(owner);
	if (!storage)
		return;

	std::vector<Entity> cards_to_destroy;

	auto add_unique = [&cards_to_destroy](const std::vector<Entity>& src)
		{
			for (Entity card : src)
			{
				if (std::find(cards_to_destroy.begin(), cards_to_destroy.end(), card) == cards_to_destroy.end())
				{
					cards_to_destroy.push_back(card);
				}
			}
		};

	add_unique(storage->data_card_hand);
	add_unique(storage->data_draw_pile);
	add_unique(storage->data_discard_pile);
	add_unique(storage->original_draw_pile);

	for (Entity card : cards_to_destroy)
	{
		ecs.destroyEntity(card);
	}

	storage->data_card_hand.clear();
	storage->data_draw_pile.clear();
	storage->data_discard_pile.clear();
	storage->original_draw_pile.clear();

	if (UIptr)
	{
		UIptr->getCardHand().reset_hand();
	}
}



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
	std::cout << "[Scene::init] tutorial_active = " << tutorial_active << '\n';

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
		std::cout << "[Tutorial] Init\n";

		clear_player_cards_for_tutorial(playerID, UIptr);

		Components::Card_Storage* storage = ecs.getComponent<Components::Card_Storage>(playerID);
		if (storage)
			storage->init();
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
		setup_tutorial_stage();
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
	
	bool allowEnemyDirector =
		(!tutorial_active) ||
		(tutorial_active && tutorial_stage == TutorialStage::DEFENSE_CARD);

	if (allowEnemyDirector)
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
	tutorial_spawned_entities.clear();

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
	//EntityFactory::free_Player();
	
	playerBarrier = Components::NULL_INDEX; // shield display
	cbs.combatSystem_free();
}

void Scene::set_tutorial_substep(int substep)
{
	tutorial_substep = substep;
}

void Scene::refresh_tutorial_text_only()
{
	print_tutorial_stage_text();
}

void Scene::setup_tutorial_stage()
{
	switch (tutorial_stage)
	{
	case TutorialStage::BASICS:
		setup_tutorial_basics();
		break;

	case TutorialStage::MOVEMENT:
		setup_tutorial_movement();
		break;

	case TutorialStage::ATTACK_CARD:
		setup_tutorial_attack();
		break;

	case TutorialStage::DEFENSE_CARD:
		setup_tutorial_defense();
		break;

	case TutorialStage::ITEM_CARD:
		setup_tutorial_item();
		break;

	case TutorialStage::EVENT_CARD:
		std::cout << "[Tutorial] Event stage not set up yet.\n";
		print_tutorial_stage_text();
		break;

	case TutorialStage::DONE:
		print_tutorial_stage_text();
		break;
	}
}

void Scene::update_tutorial()
{
}

void Scene::set_tutorial_active(bool active)
{
	tutorial_active = active;
}

bool Scene::is_tutorial_active() const
{
	return tutorial_active;
}

void Scene::clear_tutorial_spawned_entities()
{
	for (Entity e : tutorial_spawned_entities)
	{
		s32 x, y;
		if (BattleGrid.findEntityCell(e, x, y))
		{
			BattleGrid.get_pos()[x][y] = Components::NULL_INDEX;
		}

		ecs.destroyEntity(e);
	}

	tutorial_spawned_entities.clear();
}

void Scene::print_tutorial_stage_text() const
{
	std::cout << "\n==============================\n";

	switch (tutorial_stage)
	{
	case TutorialStage::BASICS:
		std::cout << "[Tutorial - Basics]\n";

		switch (tutorial_substep)
		{
		case 0:
			std::cout << "Press and hold the middle mouse button to drag across the screen.\n";
			break;
		case 1:
			std::cout << "Use the mouse wheel to zoom in and out.\n";
			break;
		default:
			std::cout << "Press SPACE to continue to MOVEMENT CARDS.\n";
			break;
		}

		std::cout << "Press SPACE to continue, E to go back, Q to restart.\n";
		break;

	case TutorialStage::MOVEMENT: 
		std::cout << "[Tutorial - Movement]\n";

		switch (tutorial_substep)
		{
		case 0:
			std::cout << "Click the player, then click any tile to move.\n";
			break;
		case 1:
			std::cout << "The blue stamina bar on the right determines how far you can go.\n";
			break;
		default:
			std::cout << "Press SPACE to continue to ATTACK CARDS.\n";
			break;
		}

		std::cout << "Press SPACE to continue, E to go back, Q to restart.\n";
		break;

	case TutorialStage::ATTACK_CARD:
		std::cout << "[Tutorial - Attack Cards]\n";

		switch (tutorial_substep)
		{
		case 0:
			std::cout << "Hover over cards to check their details. Cards are categorised by colour.\n";
			break;
		case 1:
			std::cout << "Attack cards are orange with types: Melee, AOE, Ranged.\n";
			break;
		case 2:
			std::cout << "Drag and drop a card onto the enemy. You must be within range.\n";
			break;
		case 3:
			std::cout << "Card costs are the numbers on the card, they consume PP upon cast.\n";
			break;
		case 4:
			std::cout << "Try the cards given to you. PRESS E to refresh\n";
			break;
		default:
			std::cout << "Press SPACE to continue to DEFENSE CARDS.\n";
			break;
		}

		std::cout << "Press SPACE to continue, E to go back, Q to restart.\n";
		break;

	case TutorialStage::DEFENSE_CARD:
		std::cout << "[Tutorial - Defense Cards]\n";

		switch (tutorial_substep)
		{
		case 0:
			std::cout << "Defense cards are blue and are self-cast only.\n Drag and drop the card onto the player to cast";
			break;
		case 1:
			std::cout << "Barrier provides shield, and Aura Farm gives invincibility.\n";
			break;
		case 2:
			std::cout << "Enemy intentions are displayed as icons above their heads.\n";
			std::cout << "Sword: ATTACK, Boots: MOVE, Loading: IDLE.\n";
			break;
		case 3:
			std::cout << "You may also hover enemies to see their plan.\n";
			std::cout << "Red: ATTACK, Green: MOVE, Yellow: BOTH.\n";
			break;
		case 4:
			std::cout << "Use any defense card to mitigate or reduce damage, then press END TURN.\n";
			break;
		case 5:
			std::cout << "You can also try running away... if you can... (heh)\n";
			break;
		case 6:
			std::cout << "Try the cards given to you. Press Q to reset.\n";
			break;
		default:
			std::cout << "Press SPACE to continue to ITEM CARDS.\n";
			break;
		}

		std::cout << "Press SPACE to continue, E to go back, Q to restart.\n";
		break;

	case TutorialStage::ITEM_CARD:
		std::cout << "[Tutorial - Item Cards]\n";

		switch (tutorial_substep)
		{
		case 0:
			std::cout << "Item cards are green and are self-cast only.\n";
			std::cout << "Drag and drop the card onto the player to cast.\n";
			break;
		case 1:
			std::cout << "Item cards update your stats and/or give special buffs.\n";
			std::cout << "(i.e. PP, Health, DMG, Stamina)\n";
			break;
		case 2:
			std::cout << "Some are stackable, and some pair well with other cards.\n";
			break;
		case 3:
			std::cout << "Read card descriptions for more.\n";
			break;
		default:
			std::cout << "Press SPACE to continue to EVENT CARDS.\n";
			break;
		}

		std::cout << "Press SPACE to continue, E to go back, Q to restart.\n";
		break;

	case TutorialStage::EVENT_CARD:
		std::cout << "[Tutorial - Event Cards]\n";
		std::cout << "Event card tutorial coming next.\n";
		std::cout << "Press SPACE to continue, E to go back, Q to restart.\n";
		break;

	case TutorialStage::DONE:
		std::cout << "[Tutorial Complete]\n";
		std::cout << "You have finished the tutorial.\n";
		break;
	}

	std::cout << "==============================\n";
}

void Scene::setup_tutorial_basics()
{
	clear_tutorial_spawned_entities();
	reset_tutorial_player_state();
	clear_player_cards_for_tutorial(playerID, UIptr);

	Components::Card_Storage* storage = ecs.getComponent<Components::Card_Storage>(playerID);
	if (storage)
		storage->init();

	tutorial_goal_x = 0;
	tutorial_goal_y = 0;

	print_tutorial_stage_text();
}

void Scene::setup_tutorial_movement()
{
	clear_tutorial_spawned_entities();
	reset_tutorial_player_state();
	clear_player_cards_for_tutorial(playerID, UIptr);

	Components::Card_Storage* storage = ecs.getComponent<Components::Card_Storage>(playerID);
	if (storage)
		storage->init();

	Components::TurnBasedStats* st = ecs.getComponent<Components::TurnBasedStats>(playerID);
	if (st)
	{
		st->max_movSpd = 5.f;
		st->cur_movSpd = 5.f;
	}

	tutorial_goal_x = (MAX_I / 2) + 3;
	tutorial_goal_y = MAX_J / 2;

	print_tutorial_stage_text();
}

void Scene::setup_tutorial_attack()
{
	std::cout << "[Tutorial] Attack stage init\n";

	clear_tutorial_spawned_entities();
	reset_tutorial_player_state();
	load_deck_for_tutorial("Tutorial Attack Deck");

	s32 px, py;
	if (BattleGrid.findEntityCell(playerID, px, py))
	{
		Entity e1 = beastiary.generate_enemy_from_beastiary(
			"Melee",
			{ 0.f, 0.f },
			{ 192.f, 192.f },
			Components::AnimationType::IDLE
		);

		Entity e2 = beastiary.generate_enemy_from_beastiary(
			"Melee",
			{ 0.f, 0.f },
			{ 192.f, 192.f },
			Components::AnimationType::IDLE
		);

		Animation::init_animation_for_entity(ecs, e1);
		Animation::init_animation_for_entity(ecs, e2);

		add_entity_to_scene(e1);
		add_entity_to_scene(e2);
		tutorial_spawned_entities.push_back(e1);
		tutorial_spawned_entities.push_back(e2);

		// add them to horde so combat system recognizes them
		if (TBSys.get_participant().size() > 1)
		{
			Entity horde = TBSys.get_participant()[1];
			ecs.getComponent<Components::Horde_Tag>(horde)->goons.push_back(e1);
			ecs.getComponent<Components::Horde_Tag>(horde)->goons.push_back(e2);
		}

		if (px + 2 < MAX_I && BattleGrid.get_pos()[px + 2][py] == Components::NULL_INDEX)
			BattleGrid.placeEntity(e1, px + 2, py);

		if (px + 3 < MAX_I && BattleGrid.get_pos()[px + 3][py] == Components::NULL_INDEX)
			BattleGrid.placeEntity(e2, px + 3, py);
	}

	print_tutorial_stage_text();
}

void Scene::setup_tutorial_defense()
{
	std::cout << "[Tutorial] Defense stage init\n";

	clear_tutorial_spawned_entities();
	reset_tutorial_player_state();
	load_deck_for_tutorial("Tutorial Defense Deck");

	// load tutorial defense script
	enemyDirector.loadScriptFile("Assets/levels/Tutorial_DEFENSE.txt");

	// rebuild tutorial enemies from script
	if (TBSys.get_participant().size() > 1)
	{
		Entity horde = TBSys.get_participant()[1];
		Components::Horde_Tag* hordeTag = ecs.getComponent<Components::Horde_Tag>(horde);

		if (hordeTag)
		{
			hordeTag->goons.clear();

			for (int i = 0; i < enemyDirector.getSpawnCount(); ++i)
			{
				std::string actorId = "E" + std::to_string(i);

				const int totalEnemies = enemyDirector.getSpawnCount();
				const int rangedEnemies = enemyDirector.getRangedSpawnCount();

				bool isRanged = (i >= totalEnemies - rangedEnemies);
				const std::string beastKey = isRanged ? "Ranger" : "Melee";

				Entity temp = beastiary.generate_enemy_from_beastiary(
					beastKey,
					{ 0.f, 0.f },
					{ 192.f, 192.f },
					Components::AnimationType::IDLE
				);

				Animation::init_animation_for_entity(ecs, temp);
				add_entity_to_scene(temp);
				tutorial_spawned_entities.push_back(temp);
				hordeTag->goons.push_back(temp);
				enemyDirector.bindActor(actorId, temp);
			}
		}
	}

	// place them in front of player simply
	s32 px, py;
	if (BattleGrid.findEntityCell(playerID, px, py))
	{
		int placed = 0;
		for (Entity e : tutorial_spawned_entities)
		{
			if (placed == 0 && px + 2 < MAX_I && BattleGrid.get_pos()[px + 2][py] == Components::NULL_INDEX)
			{
				BattleGrid.placeEntity(e, px + 2, py);
				placed++;
			}
			else if (placed == 1 && px + 3 < MAX_I && BattleGrid.get_pos()[px + 3][py] == Components::NULL_INDEX)
			{
				BattleGrid.placeEntity(e, px + 3, py);
				placed++;
			}
		}
	}

	print_tutorial_stage_text();
}

void Scene::setup_tutorial_item()
{
	std::cout << "[Tutorial] Item stage init\n";

	clear_tutorial_spawned_entities();
	reset_tutorial_player_state();
	load_deck_for_tutorial("Item Deck");

	print_tutorial_stage_text();
}

void Scene::set_tutorial_stage(int stage)
{
	tutorial_stage = static_cast<TutorialStage>(stage);
}

void Scene::load_deck_for_tutorial(const std::string& deck_name)
{
	clear_player_cards_for_tutorial(playerID, UIptr);

	Components::Card_Storage* storage = ecs.getComponent<Components::Card_Storage>(playerID);
	if (!storage)
		return;

	std::cout << "[Tutorial] start_decks loaded: " << card_system.start_decks.size() << '\n';
	for (const auto& deck : card_system.start_decks)
	{
		std::cout << "[Tutorial] available deck: [" << deck.name << "]\n";
	}

	bool found = false;

	for (auto& deck : card_system.start_decks)
	{
		if (deck.name == deck_name)
		{
			found = true;
			std::cout << "[Tutorial] Loading deck: " << deck.name
				<< " | card count: " << deck.cards.size() << '\n';

			for (const std::string& card_name : deck.cards)
			{
				Entity card = card_system.generate_card_from_bible(card_name);
				EntityFactory::add_card_player_deck(ecs, playerID, card);
			}
			break;
		}
	}

	if (!found)
	{
		std::cout << "[Tutorial] Deck NOT found: " << deck_name << '\n';
		return;
	}

	storage->init();

	std::vector<Entity> cards_to_hand = storage->data_draw_pile;
	storage->data_draw_pile.clear();

	for (Entity card : cards_to_hand)
	{
		EntityFactory::add_card_player_hand(ecs, playerID, card);
	}

	std::cout << "[Tutorial] Hand size after load: "
		<< storage->data_card_hand.size() << '\n';

	if (UIptr)
	{
		UIptr->getCardHand().reset_hand();
	}
}

void Scene::reset_tutorial_player_state()
{
	s32 oldX, oldY;
	if (BattleGrid.findEntityCell(playerID, oldX, oldY))
	{
		BattleGrid.get_pos()[oldX][oldY] = Components::NULL_INDEX;
	}

	s32 centerX = MAX_I / 2;
	s32 centerY = MAX_J / 2;

	// only place if grid is already initialized enough to handle it
	BattleGrid.placeEntity(playerID, centerX, centerY);

	Components::TurnBasedStats* st = ecs.getComponent<Components::TurnBasedStats>(playerID);
	if (st)
	{
		st->max_movSpd = 5.f;
		st->cur_movSpd = 5.f;
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