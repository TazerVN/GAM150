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
	void unhighlight_cells();

public:
	std::array<std::array<bool, MAX_J>, MAX_I> mov_highlight_activate;
	std::vector<AEVec2> mov_highlighted_cells;

	std::array<std::array<bool, MAX_J>, MAX_I> atk_highlight_activate;
	std::vector<AEVec2> atk_highlighted_cells;

	std::array<std::array<bool, MAX_J>, MAX_I> aoe_highlight_activate;
	std::vector<AEVec2> aoe_highlighted_cells;

	std::array<std::array<Components::RGBA, MAX_J>, MAX_I> highlight_activate;

	HighlightSystem(EventPool<highlight_tag>& eventPool, 
					Grid::GameBoard& gb,
					CombatNameSpace::CombatSystem& cbs,
					TBS::TurnBasedSystem& tbs);
	void update();
	void free();
};