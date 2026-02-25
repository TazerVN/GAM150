#pragma once
#include "../ECS/ECSystem.h"
#include "../factory/EntityFactory.h"
#include "../factory/TextureFactory.h"
#include "../system/TurnBasedSystem.h"
#include "../system/PhaseSystem.h"
#include "../util/Event.h"
#include "../system/GridSystem.h"

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

	EventPool eventPool;
	//TBS::TurnBasedSystem TBSys(eventPool);
	/*TurnBasedSystem TBSys;*/
	TBS::TurnBasedSystem TBSys;
	Grid::GameBoard BattleGrid;

public:
	void init(MeshFactory& mf, TextureFactory::TextureFactory& tf);
	void update();
	void add_entity(Entity e);
	std::vector<Entity>& entities_store();

	//GETTER
	ECS::Registry& getECS();
	PhaseSystem::GameBoardState& getGBS();
	Entity& get_playerID();
	TBS::TurnBasedSystem& getTBS();
	Grid::GameBoard& getBattleGrid();
};

void highlight_cells(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb);
void unhighlight_cells(Grid::GameBoard& gb);
AEVec2& Get_CurPart_gridPos(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb);