#pragma once
#include "../global.h"
#include "../ECS/ECSystem.h"
#include "../factory/TextureFactory.h"
#include "../system/PhaseSystem.h"
#include "../util/Event.h"
#include "../system/GridSystem.h"
#include "../system/TurnBasedSystem.h"
#include "../system/EnemyDirector.h"
#include "../system/CameraSystem.h"
#include "../system/CardSystem.h"

#include "AEEngine.h"

class Scene
{
private:
	ECS::Registry* ecs;

	size_t next_entity = 0;
	std::vector<Entity> entities;

	PhaseSystem::GameBoardState gbs;
	EventPool<highlight_tag> eventPool;
	TBS::TurnBasedSystem TBSys;
	Grid::GameBoard BattleGrid;
	Camera::CameraSystem* cameraSys;
	CardSystem* cardSys;

	EnemyDirector enemyDirector; // For CPU instructions - Zejin

public:
	void init(ECS::Registry& ECS,MeshFactory& mf, CardSystem& cs, TextureFactory::TextureFactory& tf, Camera::CameraSystem& cam , CardInteraction::CardHand& ch);
	void update();
	void add_entity(Entity e);
	std::vector<Entity>& entities_store();

	//GETTER
	ECS::Registry& getECS();
	PhaseSystem::GameBoardState& getGBS();
	TBS::TurnBasedSystem& getTBS();
	Grid::GameBoard& getBattleGrid();

};

void highlight_cells(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb, int range, highlight_tag type);
void unhighlight_cells(Grid::GameBoard& gb);
//AEVec2& Get_CurPart_gridPos(AEVec2 const& worldPos);