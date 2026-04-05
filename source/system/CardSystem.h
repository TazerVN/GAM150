//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Zejin Kendreich Dayap Chen
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				chen.z@digipen.edu
//	DATE:		5-4-2026
//===============================================================
#pragma once

#include "../types.h"

#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../system/CombatSystem.h"
#include "CardConstants.h"
#include <vector>
#include <functional>
#include <unordered_map>



//forward declarations
struct JSON_CARD;

enum class CardScriptReturn
{
	FunctionInitError,
	FunctionRunSuccess,
	FunctionNotFound,
	FunctionTargetDied
};

Entity create_ECS_card(JSON_CARD const& json_card);


class CardSystem
{
	private:
		//std::vector<Entity> cards;
	std::unordered_map<std::string, Entity> cards_map;

	public:
	std::vector<Entity>cards_vec;
	std::vector<JSON_DECK> start_decks;

	void init_cards();
	Entity generate_card_from_bible(std::string key);
	Entity generate_card_from_bible(Entity bible_id);
	Entity get_bible_id(std::string key);
	Entity getCardAtIndex(int index)const;
	size_t size() const;
};


