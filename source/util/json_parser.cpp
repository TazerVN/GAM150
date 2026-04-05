//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Zejin Kendreich Dayap Chen
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				chen.z@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================

#include "document.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <array>
#include <string>

#include "global.h"
#include "json_parser.h"
#include "document.h"
#include "prettywriter.h"
#include "ostreamwrapper.h"

int& JSON_GAME_DATA::ScoringSystem::LevelCount()
{
	return total_level_cleared;
}

int JSON_GAME_DATA::ScoringSystem::LevelCount() const
{
	return total_level_cleared;
}

bool JSON_GAME_DATA::ScoringSystem::firstLevel() const
{
	return total_level_cleared == 0;
}

void JSON_GAME_DATA::ScoringSystem::incrementLevelCleared()
{
	total_level_cleared++;
}

void JSON_GAME_DATA::ScoringSystem::reset()
{
	//pass data to scorebaord
	//parse_date_to_file();
	total_level_cleared = 0;
}


JSON_RET create_game_data()
{
	std::filesystem::path filePath(data_path_);

	std::error_code er;
	auto dir = filePath.parent_path();
	if (!dir.empty())
	{
		std::filesystem::create_directories(dir, er);
		if (er) return JSON_RET::FILE_OPEN_ERROR;
	}

	std::ofstream file(data_path_);
	if (!file.is_open()) return JSON_RET::FILE_OPEN_ERROR;


	rapidjson::OStreamWrapper osw(file);

	rapidjson::Document doc;
	doc.SetObject();
	auto& alloc = doc.GetAllocator();

	doc.AddMember("newStart", true, alloc);

	doc.AddMember("Seed", std::rand(), alloc);

	rapidjson::Value soundSettings(rapidjson::kObjectType);

	soundSettings.AddMember("sfx", 0.5f, alloc);
	soundSettings.AddMember("music", 0.5f, alloc);
	soundSettings.AddMember("ambience", 0.5f, alloc);
	doc.AddMember("SoundSetting", soundSettings, alloc);


	rapidjson::Value scoringSystem(rapidjson::kObjectType);

	scoringSystem.AddMember("levelcount", 0, alloc);


	doc.AddMember("Score", scoringSystem, alloc);

	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	doc.Accept(writer);

	file.close();

	return JSON_RET::OK;
}


JSON_RET parse_game_data()
{
	std::filesystem::path filePath(data_path_);

	std::error_code er;
	auto dir = filePath.parent_path();
	if (!std::filesystem::exists(dir))
	{
		create_game_data();
	}

	std::ifstream file(data_path_);
	if (!file.is_open())
		return JSON_RET::FILE_OPEN_ERROR;

	std::string json
	(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()
	);

	rapidjson::Document doc;
	doc.Parse(json.c_str());
	if (doc.HasParseError())
	{
		std::cerr << "Error parsing JSON: "
			<< doc.GetParseError() << '\n';
		return JSON_RET::PARSE_ERROR;
	}
	if (doc.HasMember("newStart"))
		gameData.new_Start = doc["newStart"].GetBool();
	else
		return JSON_RET::PARSE_ERROR;

	if (doc.HasMember("Seed"))
		gameData.seed = doc["Seed"].GetUint();
	else
		return JSON_RET::PARSE_ERROR;

	if (doc.HasMember("SoundSetting") && doc["SoundSetting"].IsObject())
	{
		const auto& sound = doc["SoundSetting"];

		if (sound.HasMember("sfx"))
			gameData.soundSettings.sfx = sound["sfx"].GetFloat();
		else
			return JSON_RET::PARSE_ERROR;

		if (sound.HasMember("music"))
			gameData.soundSettings.music = sound["music"].GetFloat();
		else
			return JSON_RET::PARSE_ERROR;

		if (sound.HasMember("ambience"))
			gameData.soundSettings.ambience = sound["ambience"].GetFloat();
		else
			return JSON_RET::PARSE_ERROR;
	}
	else
		return JSON_RET::PARSE_ERROR;

	if (doc.HasMember("Score") && doc["Score"].IsObject())
	{
		const auto& score = doc["Score"];

		gameData.scoringSystem.LevelCount() = score.HasMember("levelcount") ? score["levelcount"].GetUint() : 0;
	}
	else
		return JSON_RET::PARSE_ERROR;

	if (doc.HasMember("player_stats") && doc["player_stats"].IsObject())
	{
		const auto& player_stats = doc["player_stats"];

		gameData.chp = (player_stats.HasMember("CHP")) ? player_stats["CHP"].GetFloat() : 0.f;
		gameData.mhp = (player_stats.HasMember("MHP")) ? player_stats["MHP"].GetFloat() : 100.f;

		gameData.playerStats.maxPoints = (player_stats.HasMember("maxPoints")) ? player_stats["maxPoints"].GetInt() : 5;
		gameData.playerStats.points = (player_stats.HasMember("points")) ? player_stats["points"].GetInt() : 0;
		gameData.playerStats.shields = (player_stats.HasMember("shield")) ? player_stats["shield"].GetInt() : 0;
		gameData.playerStats.atkBuffHitsLeft = (player_stats.HasMember("atkBuffHitLeft")) ? player_stats["atkBuffHitLeft"].GetInt() : 0;
		gameData.playerStats.atkBuffStacks = (player_stats.HasMember("atkBuffStacks")) ? player_stats["atkBuffStacks"].GetInt() : 0;
		gameData.playerStats.atkMultiplier = (player_stats.HasMember("atkMultiplier")) ? player_stats["atkMultiplier"].GetFloat() : 1.f;
		gameData.playerStats.damageTakenMultiplier = (player_stats.HasMember("damageTakenMultiplier")) ? player_stats["damageTakenMultiplier"].GetFloat() : 1.f;
		gameData.playerStats.max_movSpd = (player_stats.HasMember("max_movSpd")) ? player_stats["max_movSpd"].GetInt() : 7;
	}
	else
	{
		gameData.playerStats.maxPoints = 5;
		gameData.playerStats.points = 0;
		gameData.playerStats.shields = 0;
		gameData.playerStats.atkBuffHitsLeft = 0;
		gameData.playerStats.atkBuffStacks = 0;
		gameData.playerStats.atkMultiplier = 1.f;
		gameData.playerStats.damageTakenMultiplier = 1.f;
		gameData.playerStats.max_movSpd = 7;
	}

	if (doc.HasMember("player_current_cards") && doc["player_current_cards"].IsArray())
	{
		Components::Card_Storage* cardStorage = ecs.getComponent<Components::Card_Storage>(playerID);
		if (cardStorage == nullptr)
			return JSON_RET::PARSE_ERROR;

		for (auto const& card : doc["player_current_cards"].GetArray())
		{
			if (card.IsString())
			{
				std::string card_name = card.GetString();
				EntityFactory::add_card_player_deck(playerID, card_system.generate_card_from_bible(card_name));
			}
		}
	}
	return JSON_RET::OK;
}

JSON_RET save_game_data()
{
	std::filesystem::path filePath(data_path_);

	std::error_code er;
	auto dir = filePath.parent_path();
	if (!dir.empty())
	{
		std::filesystem::create_directories(dir, er);
		if (er) return JSON_RET::FILE_OPEN_ERROR;
	}

	std::ofstream file(data_path_);
	if (!file.is_open()) return JSON_RET::FILE_OPEN_ERROR;


	rapidjson::OStreamWrapper osw(file);

	rapidjson::Document doc;
	doc.SetObject();
	auto& alloc = doc.GetAllocator();

	doc.AddMember("newStart", gameData.new_Start, alloc);
	doc.AddMember("Seed", gameData.seed, alloc);

	rapidjson::Value soundSettings(rapidjson::kObjectType);

	soundSettings.AddMember("sfx", gameData.soundSettings.sfx, alloc);
	soundSettings.AddMember("music", gameData.soundSettings.music, alloc);
	soundSettings.AddMember("ambience", gameData.soundSettings.ambience, alloc);
	doc.AddMember("SoundSetting", soundSettings, alloc);


	rapidjson::Value scoringSystem(rapidjson::kObjectType);
	scoringSystem.AddMember("levelcount", gameData.scoringSystem.LevelCount(), alloc);
	doc.AddMember("Score", scoringSystem, alloc);


	rapidjson::Value json_stats(rapidjson::kObjectType);
	Components::TurnBasedStats* playerS = ecs.getComponent<Components::TurnBasedStats>(playerID);
	Components::HP* player_HP = ecs.getComponent<Components::HP>(playerID);
	if (playerS != nullptr)
	{
		json_stats.AddMember("CHP", player_HP->c_value, alloc);
		json_stats.AddMember("MHP", player_HP->max_value, alloc);
		json_stats.AddMember("maxPoints", playerS->maxPoints, alloc);
		json_stats.AddMember("points", playerS->points, alloc);
		json_stats.AddMember("shield", playerS->shields, alloc);
		json_stats.AddMember("atkBuffHitLeft", playerS->atkBuffHitsLeft, alloc);
		json_stats.AddMember("atkBuffStacks", playerS->atkBuffStacks, alloc);
		json_stats.AddMember("atkMultiplier", playerS->atkMultiplier, alloc);
		json_stats.AddMember("damageTakenMultiplier", playerS->damageTakenMultiplier, alloc);
		json_stats.AddMember("max_movSpd", playerS->max_movSpd, alloc);
	}
	doc.AddMember("player_stats", json_stats, alloc);


	if (!gameData.new_Start)
	{
		rapidjson::Value player_cards(rapidjson::kArrayType);
		Components::Card_Storage* storage = ecs.getComponent<Components::Card_Storage>(playerID);
		if (storage != nullptr)
		{
			for (Entity const& card : storage->original_draw_pile)
			{
				std::string card_name = ecs.getComponent<Components::Name>(card)->value;

				player_cards.PushBack(rapidjson::Value().SetString(card_name.c_str(), alloc), alloc);
			}
		}

		doc.AddMember("player_current_cards", player_cards, alloc);
	}

	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	doc.Accept(writer);

	file.close();

	return JSON_RET::OK;
}

JSON_RET parse_bible_card_data(std::vector<JSON_CARD>& vec, char const* str)
{
	std::ifstream file(str);
	if (!file.is_open()) return JSON_RET::FILE_OPEN_ERROR;

	std::string json
	(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()
	);

	rapidjson::Document doc;
	doc.Parse(json.c_str());
	if (doc.HasParseError())
	{
		std::cerr << "Error parsing JSON: "
			<< doc.GetParseError() << '\n';
		return JSON_RET::PARSE_ERROR;
	}

	if (!doc.IsArray()) return JSON_RET::PARSE_ERROR;

	rapidjson::Value& arr = doc.GetArray();

	for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)

	{
		JSON_CARD temp_card{};

		temp_card.id = (arr[i].HasMember("id")) ? arr[i]["id"].GetInt() : -1;
		temp_card.name = (arr[i].HasMember("name")) ? arr[i]["name"].GetString() : "NULL";
		temp_card.cost = (arr[i].HasMember("cost")) ? arr[i]["cost"].GetInt() : -1;
		temp_card.value = (arr[i].HasMember("value")) ? arr[i]["value"].GetFloat() : -1.0f;
		temp_card.range = (arr[i].HasMember("range")) ? arr[i]["range"].GetInt() : 0;
		temp_card.aoe = (arr[i].HasMember("aoe")) ? arr[i]["aoe"].GetInt() : 0;
		temp_card.description = (arr[i].HasMember("description")) ? arr[i]["description"].GetString() : "No description Man?";
		temp_card.card_image = (arr[i].HasMember("png")) ? arr[i]["png"].GetString() : "NULL";

		vec.push_back(temp_card);
	}

	return JSON_RET::OK;
}

JSON_RET parse_starter_decks(std::vector<JSON_DECK>& decks, char const* str)
{
	std::ifstream file(str);
	if (!file.is_open()) return JSON_RET::FILE_OPEN_ERROR;

	std::string json
	(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()
	);

	rapidjson::Document doc;
	doc.Parse(json.c_str());
	if (doc.HasParseError())
	{
		std::cerr << "Error parsing JSON: "
			<< doc.GetParseError() << '\n';
		return JSON_RET::PARSE_ERROR;
	}

	if (!doc.HasMember("Decks")) return JSON_RET::PARSE_ERROR;

	for (auto& d : doc["Decks"].GetArray())
	{
		JSON_DECK deck;
		deck.name = d["name"].GetString();
		for (auto& card : d["cards"].GetArray())
		{
			deck.cards.push_back(card.GetString());
		}
		decks.push_back(deck);
	}

	return JSON_RET::OK;
}

JSON_RET parse_enemy_data(std::vector<JSON_ENEMY>& vec, char const* str)
{
	std::ifstream file(str);
	if (!file.is_open()) return JSON_RET::FILE_OPEN_ERROR;

	std::string json
	(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()
	);

	rapidjson::Document doc;
	doc.Parse(json.c_str());
	if (doc.HasParseError())
	{
		std::cerr << "Error parsing JSON: "
			<< doc.GetParseError() << '\n';
		return JSON_RET::PARSE_ERROR;
	}

	if (!doc.IsArray()) return JSON_RET::PARSE_ERROR;

	rapidjson::Value& arr = doc.GetArray();

	for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)

	{
		JSON_ENEMY temp_card{};

		temp_card.name = (arr[i].HasMember("name")) ? arr[i]["name"].GetString() : "NULL";
		temp_card.hp = (arr[i].HasMember("hp")) ? arr[i]["hp"].GetFloat() : -1.0f;
		temp_card.value = (arr[i].HasMember("value")) ? arr[i]["value"].GetFloat() : -1.0f;
		temp_card.range = (arr[i].HasMember("range")) ? arr[i]["range"].GetInt() : -1;
		temp_card.png = (arr[i].HasMember("png")) ? arr[i]["png"].GetString() : "NULL";

		vec.push_back(temp_card);
	}

	return JSON_RET::OK;
}