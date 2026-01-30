#include "World.h"

void World::init()
{
	card_system.init_cards(ecs);
	playerID = System::create_player(ecs, 0.f, 0.f, "Player1", 100.f);
	System::add_card_player(ecs, playerID, card_system.get_card(0));
	enemyID = System::create_player(ecs, 10.0f, 10.f, "Enemy1", 100.f);
}
Entity& World::getPlayerID()
{
	return playerID;
}
Entity& World::getEnemyID()
{
	return enemyID;
}
ECS::Registry& World::getECS()
{
	return ecs;
}