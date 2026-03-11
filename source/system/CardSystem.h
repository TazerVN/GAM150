#pragma once

#include "../ECS/ECSystem.h"
#include "CardConstants.h"
#include <vector>
#include <functional>
#include <map>

//forward declarations
struct JSON_CARD;

enum class CardSystemNames
{
	SLASH,SLASH_PLUS,
	SHOOT,SHOOT_PLUS,
	FIREBOLT,
	STEP_SLASH,
	DISK_DETONATE,
	SNIPE,
	BARRIER,BARRIER_PLUS,
	DMG_CUT,AURA_FARM,
	RECOVERY,RECOVERY_PLUS,RECOVERY_PLUS1,
	PP_UP,
	STR_DISK,
	SWIFT_FOOT,
	DATA_DRAW,
	RAND_GENERATE,
	GUST_OF_WIND,
	BLACK_HOLE
};

enum class CardScriptReturn
{
	FunctionRunSuccess,
	FunctionNotFound
};

Entity create_card(ECS::Registry& ecs, JSON_CARD const& json_card);


class CardScriptsManager
{
private:
	std::map<CardSystemNames, std::function<void(Entity)>> functions;
public:
	void init_functions();
	void add_Function(CardSystemNames cardName, Entity cardID, std::function<void(Entity)> function);
	CardScriptReturn runCardFunction(CardSystemNames card,Entity target);
};

class CardSystem
{
private:
	std::vector<Entity> cards;
	CardScriptsManager cardScriptManager;
public:
	void init_cards(ECS::Registry& ecs);
	Entity& get_card(CardSystemNames card);
	CardScriptsManager& CardScripts();
	size_t size() const;
};

//attack cards 

//defense cards
//world interaction cards