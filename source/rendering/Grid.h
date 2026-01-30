#pragma once
#include "AEEngine.h"
#include "renderSystem.h"
#include "../ECS/ECSystem.h"
#include "../util/Pathfinding.h"   // Cell, AStarResult, AStar_FindPath_Grid4

#include <vector>
#include <array>
#include <cstdint>


#define CELL_WIDTH 128
#define CELL_HEIGHT 128

#define MAX_I 16
#define MAX_J 16


namespace Grid
{

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
		bool PickCellNearest(float worldX, float worldY, int& outI, int& outJ) const;

		void RebuildWalkable();
		void ComputePath();
		void ResetColors();
		void PaintStartGoalPath();
		void ToggleBlocked(int i, int j);
	};
}

//void cells_init(Shape2D::Rectangle(&cells)[MAX_I][MAX_J]);
//void draw_cells(Shape2D::Rectangle(&cells)[MAX_I][MAX_J]);
