#pragma once
#include "../pch.h"

#include <vector>
#include <string>
#include "../system/CardConstants.h"

enum class JSON_RET
{
	FILE_OPEN_ERROR,
	PARSE_ERROR,
	OK
};

struct JSON_CARD
{
	int id = -1;
	std::string name;
	float cost{ 0.0f };
	float value{ 0.0f };
	float range{ 0.0f };
	float aoe {0.0f};
	std::string card_image;
};

struct JSON_ENEMY
{
	std::string name;
	float hp{ 0.0f };
	float value{ 0.0f };
	float range{ 0.0f };
	std::string png;
};

JSON_RET parse_card_data(std::vector<JSON_CARD>& vec,char const* str);
JSON_RET parse_enemy_data(std::vector<JSON_ENEMY>& vec, char const* str);