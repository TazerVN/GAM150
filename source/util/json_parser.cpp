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

	/*std::cout << doc["name"].GetString() << std::endl;
	std::cout << doc["card_type"].GetString() << std::endl;
	std::cout << doc["Value"].GetFloat() << std::endl;*/

	return JSON_RET::OK;
}