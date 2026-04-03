#pragma once
#include "../pch.h"

#include <vector>
#include <string>
#include "../system/CardConstants.h"

class ScoringSystem;

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

struct JSON_GAME_DATA
{
	class ScoringSystem
	{
	private:
		struct Score
		{
			int total_level_cleared = 0;
		};
		std::vector<Score> highscores;
		int total_level_cleared = 0;
	public:
		int& LevelCount();
		int LevelCount() const;
		bool firstLevel() const;
		void incrementLevelCleared();
		void reset();
	};

	struct SoundSettings
	{
		const int max_value = 100;
		unsigned int music;
		unsigned int ambience;
		unsigned int sfx;
	};

	bool new_Start;
	bool win;
	unsigned int seed;
	SoundSettings soundSettings;
	ScoringSystem scoringSystem;
};

JSON_RET create_game_data();
JSON_RET parse_game_data();
JSON_RET save_game_data();

JSON_RET parse_seed(unsigned int& seed, char const* file_loc);
JSON_RET parse_bible_card_data(std::vector<JSON_CARD>& vec,char const* str);
JSON_RET parse_starter_decks(std::vector<JSON_DECK>& decks,char const* str);
JSON_RET parse_enemy_data(std::vector<JSON_ENEMY>& vec, char const* str);