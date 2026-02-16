#include "Scene.h"

void Scene::init(MeshFactory& mf, TextureFactory::TextureFactory& tf)
{
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

	//Add enemy1
	//temp = System::create_player(ecs, mf, { 100.f,300.f }, { 192.0f,192.0f }, "Enemy1", 100.f, tf.getTexture(3));
	//System::add_card_player(ecs, temp, fa);	//add fire attack
	//System::add_card_player(ecs, temp, sa);	//add sword attack
	//add_entity(temp);
}

void Scene::update()
{
	/*if (AEInputCheckTriggered(AEVK_1))
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
	}*/
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
	return ecs;
}

Entity& Scene::get_playerID()
{
	return playerID;
}