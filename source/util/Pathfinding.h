#pragma once
#include <vector>
#include <cstdint>

struct Cell
{
	int x = 0; // i
	int y = 0; // j
	bool operator==(const Cell& o) const { return x == o.x && y == o.y; }
};

struct AStarResult
{
	std::vector<Cell> path; // includes start and goal if found
};

AStarResult AStar_FindPath_Grid4(
	int width, int height,
	const uint8_t* walkable,   // 1 = walkable, 0 = blocked, size = width*height
	Cell start, Cell goal
);
