//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
// 
//	EMAIL:		w.phyooo@digipen.edu, 
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once

#include "../ECS/Components.h"
#include <vector>
#include "AEEngine.h"
#include "types.h"

class Console
{
private:
	bool active = false;
	const unsigned int _MAX_CHAR_COUNT = 25;

	//=============Log=======================
	Entity log_box_container{Entity(-1)};
	AEVec2 lbc_pos_{ AEVec2{0,0} };
	AEVec2 lbc_size{ AEVec2{0,0} };
	Components::Color lbc_color{ 46.f / 255.f, 46.f / 255.f, 46.f / 255.f,1.f };

	//===============Text box==============
	Entity text_box_container{Entity(-1)};
	AEVec2 tc_pos_{ AEVec2{0,0} };
	AEVec2 tc_size{ AEVec2{0,0} };
	Components::Color tc_color{ 77.f / 255.f, 77.f / 255.f, 77.f / 255.f,1.f };

	Entity input_text{0};
	AEVec2 pos_{ AEVec2{0,0} };
	AEVec2 text_size_{ AEVec2{0,0} };

	std::vector<std::string> console_logs;
	void text_box_update();
	
public:
	Console() = default;
	void init();
	void update();
	void free();


};