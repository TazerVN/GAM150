#pragma once
#include "../global.h"
#include "../ECS/ECSystem.h"
#include "../factory/EntityFactory.h"
#include "../factory/TextureFactory.h"
#include "../system/PhaseSystem.h"
#include "../util/Event.h"
#include "../system/GridSystem.h"
#include "../system/TurnBasedSystem.h"

#include "AEEngine.h"

class Scene
{
private:
	ECS::Registry* ecs;

	size_t next_entity = 0;
	std::vector<Entity> entities;

	System::CardSystem card_system;
	PhaseSystem::GameBoardState gbs;
	EventPool eventPool;
	TBS::TurnBasedSystem TBSys;
	Grid::GameBoard BattleGrid;

public:
	void init(ECS::Registry& ECS,MeshFactory& mf, TextureFactory::TextureFactory& tf, CardInteraction::CardHand& ch);
	void update();
	void add_entity(Entity e);
	std::vector<Entity>& entities_store();

	//GETTER
	ECS::Registry& getECS();
	PhaseSystem::GameBoardState& getGBS();
	TBS::TurnBasedSystem& getTBS();
	Grid::GameBoard& getBattleGrid();
};

void highlight_cells(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb);
void unhighlight_cells(Grid::GameBoard& gb);
AEVec2& Get_CurPart_gridPos(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb);
AEVec2& Get_CurPart_gridPos(AEVec2 const& worldPos);