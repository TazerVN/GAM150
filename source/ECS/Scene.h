#pragma once
#include "../ECS/ECSystem.h"
#include "../factory/EntityFactory.h"
#include "../factory/TextureFactory.h"
#include "../system/TurnBasedSystem.h"
#include "../system/PhaseSystem.h"

#include "AEEngine.h"

class Scene
{
private:
	ECS::Registry ecs;
	PhaseSystem::GameBoardState gbs;

	System::CardSystem card_system;
	Entity playerID{ NULL_INDEX };
	size_t next_entity = 0;
	std::vector<Entity> entities;
public:
	void init(MeshFactory& mf, TextureFactory::TextureFactory& tf);
	void update();
	void add_entity(Entity e);
	std::vector<Entity>& entities_store();

	//GETTER
	ECS::Registry& getECS();
	PhaseSystem::GameBoardState& getGBS();
	Entity& get_playerID();
};