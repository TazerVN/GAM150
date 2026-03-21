#pragma once
class Victory_Select
{
private: 
	Entity create_Victory_card(std::string cardname, f32 x, f32 y);
public:
	std::vector<Entity>victory_cards;
	Entity dim;
	bool on = false;
	void update();
	void free();
};

