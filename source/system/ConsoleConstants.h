//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
// 
//	EMAIL:		w.phyooo@digipen.edu, 
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once
#include <array>
#include <cstddef>
#include <string>

enum class CommandTypes : char
{
	CT_HIGHLIGHT_WALKABLE,
	CT_HIGHLIGHT_POSITION,
	CT_PPUP,
	CT_PPDOWN,
	CT_KILL_ENEMIES,
	CT_COUNT
};

constexpr size_t commandCount = static_cast<size_t>(CommandTypes::CT_COUNT);

static std::array<std::string, commandCount> command_inputs = {"/highlight walkable","/highlight position", "/pp up","/pp down", "/kill enemies"};