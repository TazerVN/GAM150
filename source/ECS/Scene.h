#pragma once


#include "../types.h"
#include "../system/EnemyDirector.h"
#include "../system/TurnBasedSystem.h"
#include "../system/PhaseSystem.h"
#include "../system/GridSystem.h"
#include "../system/CombatSystem.h"
#include "../system/CameraSystem.h"
#include "../system/CardSystem.h"

#include "../factory/MeshFactory.h"
#include "../factory/TextureFactory.h"
#include "../UI/cardInteraction.h"

#include "AEEngine.h"




class Scene
{
private:
	EntityComponent::Registry* ecs;

	size_t next_entity = 0;
	std::vector<Entity> entities;

	PhaseSystem::GameBoardState gbs;
	EventPool<highlight_tag> eventPool;
	TBS::TurnBasedSystem TBSys;
	Grid::GameBoard BattleGrid;
	CombatNameSpace::CombatSystem cbs;

	Camera::CameraSystem* cameraSys;
	CardSystem* cardSys;

	EnemyDirector enemyDirector; // For CPU instructions - Zejin

public:
	void init(EntityComponent::Registry& ECS,MeshFactory& mf, CardSystem& cs, TextureFactory::TextureFactory& tf, Camera::CameraSystem& cam, CardInteraction::CardHand& ch);
	void update();
	void add_entity(Entity e);
	std::vector<Entity>& entities_store();

	//GETTER
	EntityComponent::Registry& getECS();
	PhaseSystem::GameBoardState& getGBS();
	TBS::TurnBasedSystem& getTBS();
	Grid::GameBoard& getBattleGrid();
	CombatNameSpace::CombatSystem& getCombatSystem();

};

void highlight_cells(EntityComponent::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb, CombatNameSpace::CombatSystem& cbs, int range, highlight_tag type);
void unhighlight_cells(Grid::GameBoard& gb, CombatNameSpace::CombatSystem& cbs);
//AEVec2& Get_CurPart_gridPos(AEVec2 const& worldPos);