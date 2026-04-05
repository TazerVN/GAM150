//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
// 
//	EMAIL:		w.phyooo@digipen.edu, 
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once
#include "global.h"

class Console
{
private:
	bool active = false;
	const unsigned int _MAX_CHAR_COUNT = 25;

	//=============Log=======================
	Entity log_box_container;
	AEVec2 lbc_pos_;
	AEVec2 lbc_size;
	Components::Color lbc_color{ 46.f / 255.f, 46.f / 255.f, 46.f / 255.f,1.f };

	//===============Text box==============
	Entity text_box_container;
	AEVec2 tc_pos_;
	AEVec2 tc_size;
	Components::Color tc_color{ 77.f / 255.f, 77.f / 255.f, 77.f / 255.f,1.f };

	Entity input_text;
	AEVec2 pos_;
	AEVec2 text_size_;

	std::vector<std::string> console_logs;
	void text_box_update();
	
public:
	Console();
	void init();
	void update();
	void free();


};