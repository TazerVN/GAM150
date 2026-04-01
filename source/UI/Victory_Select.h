#pragma once
#include "UI/UIObject.h"

class Victory_Select
{
private: 
	Entity create_Victory_card(std::string cardname, f32 x, f32 y);
	Entity create_Victory_card(Entity cardID, f32 x, f32 y);
public:
	std::vector<Entity>victory_cards;
	UIO::ScreenTransition dim;
	s32 z{1350};
	bool on = false;
	void update();
	void free();
};

