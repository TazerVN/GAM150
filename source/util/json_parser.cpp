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
		const int id = (arr[i].HasMember("id")) ? arr[i]["id"].GetInt() : -1;
		std::string name = (arr[i].HasMember("name")) ? arr[i]["name"].GetString() : "NULL";
		const float cost = (arr[i].HasMember("cost")) ? arr[i]["cost"].GetFloat() : -1.f;
		const char* targetting = (arr[i].HasMember("targetting")) ? arr[i]["targetting"].GetString() : "NULL";
		const char* card_tag = (arr[i].HasMember("card_type")) ? arr[i]["card_type"].GetString() : "NULL";
		const char* type = (arr[i].HasMember("type")) ? arr[i]["type"].GetString() : "NULL";
		const float value = (arr[i].HasMember("value")) ? arr[i]["value"].GetFloat() : -1.f;
		const float range = (arr[i].HasMember("range")) ? arr[i]["range"].GetFloat() : -1.f;
		const float aoe = (arr[i].HasMember("aoe")) ? arr[i]["aoe"].GetFloat() : -1.f;

		Targetting targ{};
		std::array<std::string, static_cast<size_t>(Targetting::COUNT)> targetting_names =
		{ "SELF","SINGLE_TARGET","LINE","AOE" };
		for (int j = 0; j < targetting_names.size(); ++j)
		{
			if (targetting == targetting_names[j])
			{
				targ = static_cast<Targetting>(j);
			}
		}

		CardTag cardTAG{};
		CardType cardTYPE{};

		std::array<std::string, static_cast<size_t>(CardType::COUNT)> type_names = 
		{ "SLASHING","PIERCING","BLUDGEONING","FIRE","SHIELDING","DAMAGE_REDUCTION","HEALING",
			"PP_BUFF","ATK_BUFF","MOV_BUFF","DRAWING_CARD","FORCED_MOVEMENT" };
		if (card_tag == "ATK")
		{
			cardTAG = CardTag::ATTACK;
			for (int i = 0; i < type_names.size(); ++i)
			{
				if (type == type_names[i])
				{
					cardTYPE = static_cast<CardType>(i);
				}
			}
		}
		else if (card_tag == "DEF")
		{
			cardTAG = CardTag::DEFENSE;
			for (int i = 0; i < type_names.size(); ++i)
			{
				if (type == type_names[i])
				{
					cardTYPE = static_cast<CardType>(i);
				}
			}
		}
		else if (card_tag == "ITEM")
		{
			cardTAG = CardTag::ITEM;
			for (int i = 0; i < type_names.size(); ++i)
			{
				if (type == type_names[i])
				{
					cardTYPE = static_cast<CardType>(i);
				}
			}
		}
		else if (card_tag == "EVENT")
		{
			cardTAG = CardTag::EVENT;
			for (int i = 0; i < type_names.size(); ++i)
			{
				if (type == type_names[i])
				{
					cardTYPE = static_cast<CardType>(i);
				}
			}
		}

		JSON_CARD temp_card{id,name,cost,targ,cardTAG,cardTYPE,value,range,aoe};
		vec.push_back(temp_card);
	}

	return JSON_RET::OK;
}