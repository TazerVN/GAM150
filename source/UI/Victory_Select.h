//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Pham Minh Tuan
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once


class VictoryCard;

class Victory_Select
{
	public:
	Entity create_Victory_card(std::string cardname, f32 x, f32 y);
	Entity create_Victory_card(Entity cardID, f32 x, f32 y);
	bool pending_free{false};
	std::vector<VictoryCard>victory_cards;

	UIO::TextureButton skip;
	UIO::ScreenTransition dim;
	s32 z{ 1240 };
	bool on = false;

	
	void update();
	void free();

};

class VictoryCard
{
	public:
	Entity card{ 0 };
	Entity mana{ 0 };
	VictoryCard() = default;
	VictoryCard(Victory_Select* vs, Entity cardID, f32 x, f32 y);
	VictoryCard(Victory_Select* vs, std::string cardname, f32 x, f32 y);
	void free();
};


