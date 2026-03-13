#pragma once

extern size_t playerID;
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