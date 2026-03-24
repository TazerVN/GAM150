#include "pch.h"
#include "../../Extern/rapidjson/document.h"
#include <fstream>
#include <iostream>
#include <array>

#include "json_parser.h"
#include "../../Extern/rapidjson/document.h"
#include "../../Extern/rapidjson/prettywriter.h"
#include "../../Extern/rapidjson/ostreamwrapper.h"


JSON_RET parse_date_to_file(unsigned int value ,char const* file_loc)
{
	std::ofstream file(file_loc);
	if (!file.is_open()) return JSON_RET::FILE_OPEN_ERROR;

	rapidjson::OStreamWrapper osw(file);

	rapidjson::Document doc;
	doc.SetObject();
	auto& alloc = doc.GetAllocator();
	

	doc.AddMember("Seed", value, alloc);

	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);

	doc.Accept(writer);

	file.close();

	return JSON_RET::OK;
}


JSON_RET parse_seed(unsigned int& seed, char const* file_loc)
{
	std::ifstream file(file_loc);
	if (!file.is_open()) 
		return JSON_RET::FILE_OPEN_ERROR;

	std::string json
	(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()
	);

	rapidjson::Document doc;
	doc.Parse(json.c_str());
	if (doc.HasParseError()) {
		std::cerr << "Error parsing JSON: "
			<< doc.GetParseError() << std::endl;
		return JSON_RET::PARSE_ERROR;
	}
	if (doc.HasMember("Seed"))
		seed = doc["Seed"].GetUint();
	else
		return JSON_RET::PARSE_ERROR;
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
	if (doc.HasParseError()) {
		std::cerr << "Error parsing JSON: "
			<< doc.GetParseError() << std::endl;
		return JSON_RET::PARSE_ERROR;
	}

	if (!doc.IsArray()) return JSON_RET::PARSE_ERROR;

	rapidjson::Value& arr = doc.GetArray();

	for (int i = 0; i < arr.Size(); ++i)

	{
		JSON_CARD temp_card{};

		temp_card.id = (arr[i].HasMember("id")) ? arr[i]["id"].GetInt() : -1;
		temp_card.name = (arr[i].HasMember("name")) ? arr[i]["name"].GetString() : "NULL";
		temp_card.cost = (arr[i].HasMember("cost")) ? arr[i]["cost"].GetFloat() : -1.0f;
		temp_card.value = (arr[i].HasMember("value")) ? arr[i]["value"].GetFloat() : -1.0f;
		temp_card.range = (arr[i].HasMember("range")) ? arr[i]["range"].GetFloat() : 0.0f;
		temp_card.aoe = (arr[i].HasMember("aoe")) ? arr[i]["aoe"].GetFloat() : 0.0f;
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
	if (doc.HasParseError()) {
		std::cerr << "Error parsing JSON: "
			<< doc.GetParseError() << std::endl;
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
	if (doc.HasParseError()) {
		std::cerr << "Error parsing JSON: "
			<< doc.GetParseError() << std::endl;
		return JSON_RET::PARSE_ERROR;
	}

	if (!doc.IsArray()) return JSON_RET::PARSE_ERROR;

	rapidjson::Value& arr = doc.GetArray();

	for (int i = 0; i < arr.Size(); ++i)

	{
		JSON_ENEMY temp_card{};

		temp_card.name = (arr[i].HasMember("name")) ? arr[i]["name"].GetString() : "NULL";
		temp_card.hp = (arr[i].HasMember("hp")) ? arr[i]["hp"].GetFloat() : -1.0f;
		temp_card.value = (arr[i].HasMember("value")) ? arr[i]["value"].GetFloat() : -1.0f;
		temp_card.range = (arr[i].HasMember("range")) ? arr[i]["range"].GetFloat() : -1.0f;
		temp_card.png = (arr[i].HasMember("png")) ? arr[i]["png"].GetString() : "NULL";

		vec.push_back(temp_card);
	}

	return JSON_RET::OK;
}