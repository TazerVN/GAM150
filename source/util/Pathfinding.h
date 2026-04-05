//===============================================================
//	AUTHOR:		Zejin Kendreich Dayap Chen
// 
//	EMAIL:		chen.z@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once


#include <vector>
#include <cstdint>
#include "../ECS/Components.h"



Components::AStarResult AStar_FindPath_Grid4(
	int width, int height,
	const uint8_t* walkable,   // 1 = walkable, 0 = blocked, size = width*height
	Components::GridCell start, Components::GridCell goal
);
