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

namespace Grid
{
	class GameBoard
	{
		private:
		std::vector<Entity> cells_entity_id;

		EntityComponent::Registry* ecsptr = nullptr;
		TBS::TurnBasedSystem* tbsptr;
		PhaseSystem::GameBoardState* gbsptr = nullptr;
		EventPool<highlight_tag>* evsptr = nullptr;
		CombatNameSpace::CombatSystem* cbsptr = nullptr;
		bool* win = nullptr;

		Entity cur, prev_cur;
		bool selected_part = false;
		AEVec2 offset;
		std::array<std::array<Entity, MAX_J>, MAX_I> cells;		//cell data of a grid
		//=============Data for A* Star====================
		std::array<std::array<Entity, MAX_J>, MAX_I> pos;
		//std::array<std::array<bool, MAX_J>, MAX_I> activate;

		std::array<std::array<highlight_tag, MAX_J>, MAX_I> highlight_activate;

		std::array<std::array<int, MAX_J>, MAX_I> aoe_highlight_activate;
		std::vector<AEVec2> aoe_highlighted_cells;

		Entity create_cells(AEVec2 pos, AEVec2 size, f32 rotation, AEGfxTexture* pTex, s32 x, s32 y, s8 z);

	public:
		bool in_walkable_debug = false;
		bool in_pos_debug = false;

		s32 cur_x, cur_y, prev_x, prev_y;
		uint8_t walkable[MAX_I * MAX_J]{};

		void init(TBS::TurnBasedSystem* tbsys, EventPool<highlight_tag>& evs, PhaseSystem::GameBoardState& gb, 
			CombatNameSpace::CombatSystem& cbs, AEGfxTexture* pTex, f32 ox, f32 oy, bool& _win);
		void placeEntity(Entity e, s32 x, s32 y);
		
		void trigger_play_card(s32 x, s32 y);
		void unselect_card();
		void move_trigger(s32 const& x, s32 const& y);
		void move_select(s32 const& x, s32 const& y);
		void moveEntity(Entity e, s32 x, s32 y);

		// helper functions for CPU - Zejin 
		bool findEntityCell(Entity e, s32& outX, s32& outY) const;
		bool moveEntityAI(Entity e, s32 x, s32 y);

		void update(EntityComponent::Registry& ecs, Entity camera);
		void updateCell(s32 x, s32 y);
		void func_aoe_hightlight_cells(s32 x , s32 y);

		//getters
		std::array<std::array<Entity, MAX_J>, MAX_I>& get_pos();
		std::array<std::array<highlight_tag, MAX_J>, MAX_I>& activate_highlight();
		std::array<std::array<int, MAX_J>, MAX_I>& activate_aoe_highlight();

		bool selected_player() const;
		void unselect_movement();
		AEVec2& Get_CurPart_gridPos();
		AEVec2 GetOffsetPos();

		s32 grid_dist_manhattan(s32 const& x1, s32 const& x2, s32 const& y1, s32 const& y2);
		s32 grid_dist_chebyshev(s32 const& x1, s32 const& x2, s32 const& y1, s32 const& y2);

		void debug_print();

		void gameboard_free();
	};
	
	//class Grid
	//{
	//	public:
	//	//Shape2D::Rectangle cells[MAX_I][MAX_J];
	//	int cells_color[MAX_I][MAX_J];
	//	f32 cells_alpha[MAX_I][MAX_J];
	//	Grid();		//default constructor
	//	void update(float camX, float camY, int winW, int winH); //so grid takes parameters of the world
	//	//void render() override;

	//	// A* click state
	//	bool hasStart = false;
	//	bool hasGoal = false;
	//	int start_i = -1, start_j = -1;
	//	int goal_i = -1, goal_j = -1;

	//	//std::vector<Cell> path;

	//	// Optional walls (leave all false for now)
	//	bool blocked[MAX_I][MAX_J]{};
	//	uint8_t walkable[MAX_I * MAX_J]{};

	//	private:
	//	//zejin this is ur old A star code
	//	/*bool PickCellNearest(float worldX, float worldY, int& outI, int& outJ) const;
	//	void RebuildWalkable();
	//	void ComputePath();
	//	void ResetColors();
	//	void PaintStartGoalPath();
	//	void ToggleBlocked(int i, int j);*/
	//};

}

//void cells_init(Shape2D::Rectangle(&cells)[MAX_I][MAX_J]);
//void draw_cells(Shape2D::Rectangle(&cells)[MAX_I][MAX_J]);
