#pragma once
#include "../ECS/ECSystem.h"
#include "../cards/Systems.h"
#include "../system/TurnBasedSystem.h"
#include "../rendering/TextureFactory.h"
#include "AEEngine.h"

class Scene
{
private:
	ECS::Registry ecs;
	System::CardSystem card_system;

	size_t next_entity = 0;
	std::vector<Entity> entities;
public:
	void init(MeshFactory& mf, TextureFactory::TextureFactory& tf);
	void update();
	void add_entity(Entity e);
	std::vector<Entity>& entities_store();
	ECS::Registry& getECS();
};