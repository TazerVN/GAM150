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
	int id;
	std::string name;
	float cost;
	Targetting targetting;
	CardTag card_tag;
	CardType card_type;
	float value;
	float range;
	float aoe;
};

JSON_RET parse_data(std::vector<JSON_CARD>& vec,char const* str);