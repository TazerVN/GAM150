#pragma once
#include "AEEngine.h"
#include "../ECS/ECSystem.h"
#include "../rendering/Mesh_factory.h"
#include "../rendering/TextureFactory.h"
#include "../util/Pathfinding.h"   // Cell, AStarResult, AStar_FindPath_Grid4

#include <vector>
#include <array>
#include <cstdint>


#define CELL_WIDTH 128
#define CELL_HEIGHT 128

#define MAX_I 10
#define MAX_J 10


namespace Grid
{
	class Grid2D
	{
		private:
			//=============Render====================
		Entity cur;
		s32 cur_x, cur_y;
		AEVec2 offset;
		std::array<std::array<Entity, MAX_J>, MAX_I> cells;		//cell data of a grid
		//=============Data for A* Star====================
		std::array<std::array<bool, MAX_J>, MAX_I> walkable;
		std::array<std::array<Entity, MAX_J>, MAX_I> pos;
		std::array<std::array<bool, MAX_J>, MAX_I> activate;

		Entity create_cells(ECS::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, f32 rotation, AEGfxTexture* pTex, s32 x, s32 y, s8 z);
		public:
		void init(ECS::Registry& ecs, MeshFactory& mf, AEGfxTexture* pTex, f32 ox, f32 oy);
		void placeEntity(ECS::Registry& ecs, Entity e, s32 x, s32 y);
		void moveEntity(ECS::Registry& ecs, Entity e, s32 x, s32 y);
		void update(ECS::Registry& ecs);
		void updateCell(ECS::Registry& ecs, s32 x, s32 y);
	};

	class Grid
	{
		public:
		//Shape2D::Rectangle cells[MAX_I][MAX_J];
		int cells_color[MAX_I][MAX_J];
		f32 cells_alpha[MAX_I][MAX_J];
		Grid();		//default constructor
		void update(float camX, float camY, int winW, int winH); //so grid takes parameters of the world
		//void render() override;

		// A* click state
		bool hasStart = false;
		bool hasGoal = false;
		int start_i = -1, start_j = -1;
		int goal_i = -1, goal_j = -1;

		//std::vector<Cell> path;

		// Optional walls (leave all false for now)
		bool blocked[MAX_I][MAX_J]{};
		uint8_t walkable[MAX_I * MAX_J]{};

		private:
		//zejin this is ur old A star code
		/*bool PickCellNearest(float worldX, float worldY, int& outI, int& outJ) const;
		void RebuildWalkable();
		void ComputePath();
		void ResetColors();
		void PaintStartGoalPath();
		void ToggleBlocked(int i, int j);*/
	};

}

//void cells_init(Shape2D::Rectangle(&cells)[MAX_I][MAX_J]);
//void draw_cells(Shape2D::Rectangle(&cells)[MAX_I][MAX_J]);
