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
	std::string description;
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

struct JSON_DECK
{
	std::string name;
	std::vector<std::string> cards;
};

JSON_RET parse_date_to_file(unsigned int value, char const* file_loc);
JSON_RET parse_seed(unsigned int& seed, char const* file_loc);
JSON_RET parse_bible_card_data(std::vector<JSON_CARD>& vec,char const* str);
JSON_RET parse_starter_decks(std::vector<JSON_DECK>& decks,char const* str);
JSON_RET parse_enemy_data(std::vector<JSON_ENEMY>& vec, char const* str);