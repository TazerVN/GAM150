#pragma once
#include "../types.h"
#include "../system/EnemyDirector.h"
#include "../system/PhaseSystem.h"
#include "../system/GridSystem.h"
#include "../system/CameraSystem.h"
#include "../system/CardSystem.h"
#include "../system/HightlightSystem.h"
#include "../factory/MeshFactory.h"
#include "../factory/TextureFactory.h"
#include "../UI/IntentionDisplay.h"
#include "AEEngine.h"


// Forward declarations
namespace TBS { class TurnBasedSystem; }
namespace Grid { class GameBoard; }
namespace CombatNameSpace { class CombatSystem; }
namespace Text { class NameTag; }
namespace EntityFactory { class InteractableNode; }
namespace Camera { class CameraSystem; }
namespace UI { class UIManager; }

class IntentionDisplaySystem;
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
	UI::UIManager* UIptr;

	EnemyDirector enemyDirector; // For CPU instructions - Zejin
	Entity playerBarrier = Components::NULL_INDEX; // shield display
	IntentionDisplaySystem intentDisplaySystem;

	HighlightSystem highlightSystem = HighlightSystem(eventPool, BattleGrid, cbs, TBSys);
	bool _win = false;

public:
	void init(Camera::CameraSystem&, UI::UIManager&);
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

void highlight_cells(bool,TBS::TurnBasedSystem&, Grid::GameBoard&, CombatNameSpace::CombatSystem&, int, highlight_tag);
void unhighlight_cells(Grid::GameBoard& gb, CombatNameSpace::CombatSystem& cbs);
//AEVec2& Get_CurPart_gridPos(AEVec2 const& worldPos);