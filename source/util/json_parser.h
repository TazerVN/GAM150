#pragma once
enum class JSON_RET
{
	FILE_OPEN_ERROR,
	PARSE_ERROR,
	OK
};
JSON_RET parse_data();