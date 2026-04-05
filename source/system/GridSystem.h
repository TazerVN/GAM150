//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Pham Minh Tuan
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once
#include "../types.h"
#include "AEEngine.h"

#include "../ECS/ECSystem.h"
#include "../factory/MeshFactory.h"
#include "../factory/TextureFactory.h"
#include "../util/Pathfinding.h"   // Cell, AStarResult, AStar_FindPath_Grid4
#include "../util/Event.h"


#include <vector>
#include <array>
#include <cstdint>


#define CELL_WIDTH 128
#define CELL_HEIGHT 128

#define MAX_I 15
#define MAX_J 20

//forward declaring tbs so that linker can use it 
namespace TBS { class TurnBasedSystem; }
namespace PhaseSystem { class GameBoardState; }
namespace CombatNameSpace {class CombatSystem;}
class HighlightSystem;

namespace Grid
{
	class GameBoard
	{
	private:
		s32 hx, hy;

		bool mouse_on_board = false;

		std::vector<Entity> cells_entity_id;

		TBS::TurnBasedSystem* tbsptr;
		PhaseSystem::GameBoardState* gbsptr = nullptr;
		EventPool<highlight_tag>* evsptr = nullptr;
		CombatNameSpace::CombatSystem* cbsptr = nullptr;
		HighlightSystem* hlptr = nullptr;

		Entity cur, prev_cur;
		bool selected_part = false;
		AEVec2 offset;
		std::array<std::array<Entity, MAX_J>, MAX_I> cells;		//cell data of a grid
		//=============Data for A* Star====================
		std::array<std::array<Entity, MAX_J>, MAX_I> pos;
		

		Entity create_cells(AEVec2 pos, AEVec2 size, f32 rotation, AEGfxTexture* pTex, s32 x, s32 y, s32 z);

	public:
		bool in_walkable_debug = false;
		bool in_pos_debug = false;
		int placementDirection = 0;

		s32 cur_x, cur_y, prev_x, prev_y;
		uint8_t walkable[MAX_I * MAX_J]{};

		void init(TBS::TurnBasedSystem* tbsys, EventPool<highlight_tag>& evs, PhaseSystem::GameBoardState& gb,
			CombatNameSpace::CombatSystem& cbs, HighlightSystem& hl, AEGfxTexture* pTex, f32 ox, f32 oy);
		void placeEntity(Entity e, s32 x, s32 y);

		void trigger_play_card(s32 x, s32 y);
		void unselect_card();
		void move_trigger(s32 const& x, s32 const& y);
		void move_select(s32 const& x, s32 const& y);
		void moveEntity(Entity e, s32 x, s32 y);

		// helper functions for CPU - Zejin 
		bool findEntityCell(Entity e, s32& outX, s32& outY) const;
		bool moveEntityAI(Entity e, s32 x, s32 y, int max_move = 5);
		void setEnemyAnimationPhase();

		void update();
		void updateCell(s32 x, s32 y);

		void draw_movement_trail(s32 x, s32 y);
		void func_aoe_hightlight_cells(s32 x, s32 y);

		bool getGridCellFromMouse(s32& out_x, s32& out_y) const;

		//getters
		std::array<std::array<Entity, MAX_J>, MAX_I>& get_pos();

		bool selected_player() const;
		void unselect_movement();
		AEVec2& Get_CurPart_gridPos();
		AEVec2 GetOffsetPos();

		void debug_print();

		void gameboard_free();
	};
}