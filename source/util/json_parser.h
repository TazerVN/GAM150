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
	float cost = 0.0f;
	float value = 0.0f;
	float range = 0.0f;
	float aoe = 0.0f;
};

JSON_RET parse_data(std::vector<JSON_CARD>& vec,char const* str);