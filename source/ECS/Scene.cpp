#include "Scene.h"

void Scene::init(MeshFactory& mf)
{
	card_system.init_cards(ecs);
	playerID = System::create_player(ecs, mf, { 0.f,0.f }, { 10.0f,10.0f }, "Player1", 100.f);
	//add cards to the player
	System::add_card_player(ecs, playerID, card_system.get_card(0));
	enemyID = System::create_player(ecs, mf, { 100.f,0.f }, { 20.0f,20.0f }, "Enemy1", 100.f);
}

void Scene::update()
{
	if (AEInputCheckTriggered(AEVK_1))
	{
		tbs.add_participant(ecs, playerID);
	}
	if (AEInputCheckTriggered(AEVK_2))
	{
		tbs.add_participant(ecs, enemyID);
	}
	if (AEInputCheckTriggered(AEVK_F1))
	{
		tbs.start(ecs);
	}
	if (tbs.active() && AEInputCheckTriggered(AEVK_K))
	{
		tbs.end();
	}
}

Entity& Scene::getPlayerID()
{
	return playerID;
}
Entity& Scene::getEnemyID()
{
	return enemyID;
}
ECS::Registry& Scene::getECS()
{
	return ecs;
}