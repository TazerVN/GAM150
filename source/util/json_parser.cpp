#include "pch.h"
#include "json_parser.h"
#include "../../Extern/rapidjson/document.h"
#include <fstream>
#include <iostream>
#include <array>

JSON_RET parse_data(std::vector<JSON_CARD>& vec, char const* str)
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

		vec.push_back(temp_card);
	}

	return JSON_RET::OK;
}