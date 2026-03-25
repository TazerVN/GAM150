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

//enum class CardSystemNames
//{
//	SLASH,SLASH_PLUS,
//	SHOOT,SHOOT_PLUS,
//	FIREBOLT,
//	STEP_SLASH,
//	DISK_DETONATE,
//	SNIPE,
//	BARRIER,BARRIER_PLUS,
//	DMG_CUT,AURA_FARM,
//	RECOVERY,RECOVERY_PLUS,RECOVERY_PLUS1,
//	PP_UP,
//	STR_DISK,
//	SWIFT_FOOT,
//	DATA_DRAW,
//	RAND_GENERATE,
//	GUST_OF_WIND,
//	BLACK_HOLE
//};

enum class CardScriptReturn
{
	FunctionInitError,
	FunctionRunSuccess,
	FunctionNotFound,
	FunctionTargetDied
};

Entity create_ECS_card(EntityComponent::Registry& ecs, JSON_CARD const& json_card);

////script manager for those cards that require extra steps
//class CardScriptsManager
//{
//	private:
//	std::unordered_map<std::string, std::function<COMBAT_SYSTEM_RETURN_TAG(Entity)>> functions;
//	//std::map<CardSystemNames, std::function<void(Entity)>> functions;
//	public:
//	void add_Function(std::string name, Entity cardID, std::function<COMBAT_SYSTEM_RETURN_TAG(Entity)> function);
//	CardScriptReturn runCardFunction(std::string card_name, Entity target);
//};

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

//attack cards 

//defense cards
//world interaction cards

