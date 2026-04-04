#pragma once

#include "util/Event.h"
#include "types.h"
namespace Grid {class GameBoard;}
namespace CombatNameSpace { class CombatSystem; }
namespace TBS { class TurnBasedSystem; }

class HighlightSystem
{
private: 
	EventPool<highlight_tag>* evntptr;
	Grid::GameBoard* gbptr;
	CombatNameSpace::CombatSystem* cbsptr;
	TBS::TurnBasedSystem* tbsptr;

	void highlight_cells(Targetting targetting, int range, highlight_tag type);

public:
	void unhighlight_cells();
	void unhighlight_atk_cells();
	void unhighlight_mov_cells();
	void highlight_enemy_cells(Entity target);
	void unhighlight_enemy_cells();
	void unhighlight_enemy_cells(Entity target);

	void clear_enemy_cells();
	void refresh_enmey_cells();
	void refresh_enmey_cell(Entity);
	/*void highlight_enemy_attack(Entity target,f32 range);*/

	std::vector<Components::GridCell> mov_highlighted_cells;

	std::vector<Components::GridCell> atk_highlighted_cells;

	std::array<std::array<int, MAX_J>, MAX_I> aoe_highlight_activate;
	std::vector<Components::GridCell> aoe_highlighted_cells;

	std::array<std::array<bool, MAX_J>, MAX_I> move_trail_highlight_activate;
	std::vector<Components::GridCell> move_trail_highlighted_cells;

	std::array<std::array<bool, MAX_J>, MAX_I> enemy_mov_highlight_activate;
	std::unordered_map<Entity,std::vector<Components::GridCell>> enemy_mov_highlighted_cells;

	std::array<std::array<bool, MAX_J>, MAX_I> enemy_atk_highlight_activate;
	std::unordered_map<Entity, std::vector<Components::GridCell>> enemy_atk_highlighted_cells;

	std::array<std::array<Components::RGBA, MAX_J>, MAX_I> highlight_activate;

	HighlightSystem(EventPool<highlight_tag>& eventPool, 
					Grid::GameBoard& gb,
					CombatNameSpace::CombatSystem& cbs,
					TBS::TurnBasedSystem& tbs);
	
	void update();
	void free();
};