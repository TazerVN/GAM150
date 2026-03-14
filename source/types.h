#pragma once

#include <cstddef>


using Entity = size_t;

enum class highlight_tag
{
	UNHIGHLIGHTED = 0,
	ATTACK_HIGHLIGHT = 1,
	MOVE_HIGHLIGHT = 2,
	AOE_HIGHTLIGHT
};

enum class PC_RETURN_TAG
{
	INVALID = -1,
	VALID = 0
};


typedef enum { MESH_CENTER = 0, MESH_CORNER = 1, MESH_SPRITE_SHEET } MESH_POS;
typedef enum { COLOR = 0, TEXTURE = 1 } RENDER_MODE;
typedef enum { MESH_RECTANGLE_CENTER = 0, MESH_RECTANGLE_CORNER, MESH_CIRCLE, MESH_LINE, MESH_SPRITE } MESH_TYPE;


