#include "pch.h"
#include "Scene.h"
#include <ctime> // for randomiser -Zejin (FOR NOW...)
#include <cstdlib> // randomiser part 2

// STEVEN HERE IS THE HELPER - Zejin
Entity spawnEnemyAndBind(EntityComponent::Registry& ecs,
	MeshFactory& mf,
	TextureFactory::TextureFactory& tf,
	EnemyDirector& enemyDirector,
	const std::string& actorId,
	const char* name,
	AEVec2 pos,
	Entity fa,
	Entity sa)
{
	Entity e = EntityFactory::create_actor_normal(ecs, mf, pos, { 192.0f, 192.0f }, name, 100.f, tf.getTextureChar(1), Components::AnimationType::IDLE);
	enemyDirector.bindActor(actorId, e);
	return e;
}




void Scene::init(EntityComponent::Registry&ECS,MeshFactory& mf, CardSystem& cs, TextureFactory::TextureFactory& tf, Camera::CameraSystem& cam, CardInteraction::CardHand& ch)

{
	std::srand(static_cast<unsigned>(std::time(nullptr)));
	cameraSys = &cam;
	cardSys = &cs;

	s32 w_width = AEGfxGetWindowWidth();
	s32 w_height = AEGfxGetWindowHeight();

	//must init appoint ecs first
	ecs = &ECS;
	//add cards to the player
	Entity temp; 
	
	//Add player
	temp = EntityFactory::create_actor_spritesheet(*ecs, mf, { 0.f,0.f }, { 192.0f,192.0f }, "Player", 100.f, tf.getTextureChar(2), Components::AnimationType::NONE);
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
	EntityFactory::add_card_player_deck(*ecs, temp, cardSys->generate_card_from_bible(*ecs, "Slash"));
	EntityFactory::add_card_player_deck(*ecs, temp, cardSys->generate_card_from_bible(*ecs, "Black Hole"));


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

		temp = EntityFactory::create_actor_normal(*ecs, mf, spawnPos, { 192.0f,192.0f }, enemyName.c_str(), 100.f, tf.getTextureChar(1), Components::AnimationType::IDLE);

		add_entity(temp);                  // adds to scene/world
		enemyDirector.bindActor(actorId, temp);
	}

	//New Vector for TBS
	std::vector<Entity> tbsParticipants;
	tbsParticipants.push_back(playerID);
	tbsParticipants.push_back(horde);

	////Add enemy0
	//temp = EntityFactory::create_actor_normal(*ecs, mf, { 100.f,100.f }, { 192.0f,192.0f }, "Enemy0", 100.f, tf.getTextureChar(0), Components::AnimationType::IDLE);
	//EntityFactory::add_card_player_hand(*ecs, temp, cardSys->generate_card_from_bible(*ecs,"Slash"));	//add fire attack
	//EntityFactory::add_card_player_hand(*ecs, temp, cardSys->generate_card_from_bible(*ecs,"Slash"));	//add sword attack
	//add_entity(temp);
	//enemyDirector.bindActor("E0", temp);		// enemy now bound as E0

	////Add enemy1
	//temp = EntityFactory::create_actor_normal(*ecs, mf, { 100.f,100.f }, { 192.0f,192.0f }, "Enemy", 100.f, tf.getTextureChar(1), Components::AnimationType::IDLE);
	//EntityFactory::add_card_player_hand(*ecs, temp, cardSys->generate_card_from_bible(*ecs,"Slash"));	//add fire attack
	//EntityFactory::add_card_player_hand(*ecs, temp, cardSys->generate_card_from_bible(*ecs,"Slash"));	//add sword attack
	//add_entity(temp);
	//enemyDirector.bindActor("E1", temp);		// enemy now bound as E1

	cbs.init(*ecs, gbs, BattleGrid ,TBSys, ch, eventPool);
	TBSys.init(*ecs,eventPool, BattleGrid, gbs, cbs, cs, ch ,entities);
	BattleGrid.init(*ecs, mf, &TBSys, eventPool, gbs, cbs, tf.getTextureFloor(0), 0, w_height / 3);

	
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
			BattleGrid.placeEntity(*ecs, entities[i], x, y);
		}
		else
		{
			std::cout << "[Scene] Failed to place entity " << entities[i] << " on empty tile.\n";
		}
	}


}

void Scene::update()
{
	TBSys.update(*ecs);
	enemyDirector.update(*ecs, gbs, TBSys, BattleGrid, playerID);
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
			Entity card_ID = TBSys.draw_card(*ecs, TBSys.current(), TBSys.get_selected_cardhand_index());
			f32& card_range = ecs->getComponent<Components::Targetting_Component>(card_ID)->range;

			highlight_cells(*ecs, TBSys, BattleGrid, cbs , card_range, highlight_type);

			break;
		}
		case highlight_tag::MOVE_HIGHLIGHT:
		{
			f32& range = ecs->getComponent<Components::TurnBasedStats>(TBSys.current())->cur_movSpd;
			highlight_cells(*ecs, TBSys, BattleGrid, cbs, range, highlight_type);
		}
			break;
		default:
			break;
		}

		eventPool.template_pool[HIGHLIGHT_EVENT].triggered = false;
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

EntityComponent::Registry& Scene::getECS()
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

CombatNameSpace::CombatSystem& Scene::getCombatSystem()
{
	return cbs;
}

void highlight_cells(EntityComponent::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb, CombatNameSpace::CombatSystem& cbs ,int range,highlight_tag type)
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
				cbs.get_highlighted_cell().push_back({ cur_part_pos.x + i , cur_part_pos.y + j });
				gb.activate_highlight()[cur_part_pos.x + i][cur_part_pos.y + j] = type;
			}
			if (i + j <= range &&cur_part_pos.x - i >= 0 && cur_part_pos.y - j >= 0)
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