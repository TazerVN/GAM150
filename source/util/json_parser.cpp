#include "json_parser.h"
#include "../../Extern/rapidjson/document.h"
#include <fstream>
#include <iostream>
JSON_RET parse_data()
{
	std::ifstream file("example.json");
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

}