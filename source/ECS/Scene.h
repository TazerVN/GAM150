#pragma once


#include "../types.h"
#include "../system/EnemyDirector.h"
//#include "../system/TurnBasedSystem.h"
#include "../system/PhaseSystem.h"
#include "../system/GridSystem.h"
//#include "../system/CombatSystem.h"
#include "../system/CameraSystem.h"
#include "../system/CardSystem.h"
#include "../factory/MeshFactory.h"
#include "../factory/TextureFactory.h"
//#include "../UI/cardInteraction.h"

#include "AEEngine.h"


// Forward declarations
namespace TBS { class TurnBasedSystem; }
namespace Grid { class GameBoard; }
namespace CombatNameSpace { class CombatSystem; }
namespace Text { class NameTag; }
namespace EntityFactory { class InteractableNode; }
namespace Camera { class CameraSystem; }
class CardSystem;
class EnemyDirector;


class Scene
{
private:

	size_t next_entity = 0;
	std::vector<Entity> entities;

	PhaseSystem::GameBoardState gbs;
	EventPool<highlight_tag> eventPool;
	TBS::TurnBasedSystem TBSys;
	Grid::GameBoard BattleGrid;
	CombatNameSpace::CombatSystem cbs;
	//Text::NameTag nameTags;
	EntityFactory::InteractableNode iNodes;


	Camera::CameraSystem* cameraSys;
	CardSystem* cardSys;

	EnemyDirector enemyDirector; // For CPU instructions - Zejin

	bool _win = false;

public:
	void init(EntityComponent::Registry& ECS,MeshFactory& mf, CardSystem& cs, TextureFactory::TextureFactory& tf, Camera::CameraSystem& cam, CardInteraction::CardHand& ch);
	void update();
	void add_entity_to_scene(Entity e);
	std::vector<Entity>& entities_store();

	void scene_free();

	//GETTER
	PhaseSystem::GameBoardState& getGBS();
	TBS::TurnBasedSystem& getTBS();
	Grid::GameBoard& getBattleGrid();
	CombatNameSpace::CombatSystem& getCombatSystem();
	bool win() const;

};

void highlight_cells(EntityComponent::Registry& ecs, TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb, CombatNameSpace::CombatSystem& cbs, int range, highlight_tag type);
void unhighlight_cells(Grid::GameBoard& gb, CombatNameSpace::CombatSystem& cbs);
//AEVec2& Get_CurPart_gridPos(AEVec2 const& worldPos);