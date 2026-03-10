#pragma once
#include <vector>

enum class JSON_RET
{
	FILE_OPEN_ERROR,
	PARSE_ERROR,
	OK
};

struct JSON_CARD
{
	char* name;
	int targetting;
	int card_tag;
	float value;
	float range;
	float aoe;
};

JSON_RET parse_data(std::vector<JSON_CARD>& vec,char const* str);