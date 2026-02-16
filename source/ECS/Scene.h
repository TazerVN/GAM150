#pragma once
#include "../ECS/ECSystem.h"
#include "../factory/EntityFactory.h"
#include "../system/TurnBasedSystem.h"
#include "../factory/TextureFactory.h"
#include "AEEngine.h"

class Scene
{
private:
	ECS::Registry ecs;
	System::CardSystem card_system;
	Entity playerID{ NULL_INDEX };
	size_t next_entity = 0;
	std::vector<Entity> entities;
public:
	void init(MeshFactory& mf, TextureFactory::TextureFactory& tf);
	void update();
	void add_entity(Entity e);
	std::vector<Entity>& entities_store();
	ECS::Registry& getECS();
	Entity& get_playerID();
};