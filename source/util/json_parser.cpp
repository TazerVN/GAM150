#include "json_parser.h"
#include "../../Extern/rapidjson/document.h"
#include <fstream>
#include <iostream>

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
		const char* name = (arr[i].HasMember("name")) ? arr[i]["name"].GetString() : "NULL";
		const float cost = (arr[i].HasMember("value")) ? arr[i]["value"].GetFloat() : -1.f;
		const char* targetting = (arr[i].HasMember("targetting")) ? arr[i]["targetting"].GetString() : "NULL";
		const char* card_type = (arr[i].HasMember("card_type")) ? arr[i]["card_type"].GetString() : "NULL";
		const char* damage_type = (arr[i].HasMember("damage_type")) ? arr[i]["damage_type"].GetString() : "NULL";
		const float value = (arr[i].HasMember("value")) ? arr[i]["value"].GetFloat() : -1.f;
		const float range = (arr[i].HasMember("range")) ? arr[i]["range"].GetFloat() : -1.f;
		const float aoe = (arr[i].HasMember("aoe")) ? arr[i]["aoe"].GetFloat() : -1.f;


	}
	/*std::cout << doc["name"].GetString() << std::endl;
	std::cout << doc["card_type"].GetString() << std::endl;
	std::cout << doc["Value"].GetFloat() << std::endl;*/

	return JSON_RET::OK;
}