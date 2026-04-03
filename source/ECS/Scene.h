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
	std::vector<Entity> tutorial_spawned_entities; // for tutorial enemies

	HighlightSystem highlightSystem = HighlightSystem(eventPool, BattleGrid, cbs, TBSys);
	bool _win = false;

	enum class TutorialStage
	{
		BASICS = 0,
		MOVEMENT,
		ATTACK_CARD,
		DEFENSE_CARD,
		ITEM_CARD,
		EVENT_CARD,
		WIN_TRANSITION,
		DONE
	};

	bool tutorial_active = false;
	TutorialStage tutorial_stage = TutorialStage::BASICS;
	int tutorial_substep = 0;

	s32 tutorial_goal_x = 0;
	s32 tutorial_goal_y = 0;


	void update_tutorial();

	void setup_tutorial_stage();
	void setup_tutorial_basics();
	void setup_tutorial_movement();
	void setup_tutorial_attack();
	void setup_tutorial_defense();
	void setup_tutorial_item();
	void setup_tutorial_event();

	void clear_tutorial_spawned_entities();

	void print_tutorial_stage_text() const;


public:
	void init(Camera::CameraSystem&, UI::UIManager&);
	void update();
	void add_entity_to_scene(Entity e);
	std::vector<Entity>& entities_store();
	void scene_free();

	void set_tutorial_stage(int stage);
	void set_tutorial_substep(int substep);
	void reset_tutorial_player_state();
	void refresh_tutorial_text_only();

	void load_deck_for_tutorial(const std::string& deck_name);
	void set_tutorial_active(bool active);
	bool is_tutorial_active() const;

	//GETTER
	PhaseSystem::GameBoardState& getGBS();
	TBS::TurnBasedSystem& getTBS();
	Grid::GameBoard& getBattleGrid();
	CombatNameSpace::CombatSystem& getCombatSystem();
	bool win() const;

};