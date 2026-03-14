#pragma once

#include "../types.h"

#include "../ECS/ECSystem.h"
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

Entity create_card(EntityComponent::Registry& ecs, JSON_CARD const& json_card);

//script manager for those cards that require extra steps
class CardScriptsManager
{
	private:
	std::unordered_map<std::string, std::function<COMBAT_SYSTEM_RETURN_TAG(Entity)>> functions;
	//std::map<CardSystemNames, std::function<void(Entity)>> functions;
	public:
	void add_Function(std::string name, Entity cardID, std::function<COMBAT_SYSTEM_RETURN_TAG(Entity)> function);
	CardScriptReturn runCardFunction(std::string card_name, Entity target);
};

class CardSystem
{
	private:
		//std::vector<Entity> cards;
	std::unordered_map<std::string, Entity> cards;
	CardScriptsManager cardScriptManager;
	public:
	void init_cards(EntityComponent::Registry& ecs);
	Entity generate_card_from_bible(EntityComponent::Registry& ecs, std::string key);
	size_t size() const;
};

//attack cards 

//defense cards
//world interaction cards

