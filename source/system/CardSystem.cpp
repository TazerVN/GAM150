//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Zejin Kendreich Dayap Chen
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				chen.z@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================

#include "CardSystem.h"
#include "../util/json_parser.h"
#include "../ECS/Components.h"
#include "../global.h"
#include <vector>

// Helpers
static int get_category_from_id(int id) { return id / 1000; }
static int get_family_from_id(int id) { return (id / 100) % 10; }
static int get_targetting_from_id(int id) { return (id / 10) % 10; }

static CardTag decode_card_tag(int id)
{
	switch (get_category_from_id(id))
	{
	case 1: return CardTag::ATTACK;
	case 2: return CardTag::DEFENSE;
	case 3: return CardTag::ITEM;
	case 4: return CardTag::EVENT;
	default: return CardTag::ITEM;
	}
}

static Targetting decode_targetting(int id)
{
	switch (get_targetting_from_id(id))
	{
	case 0: return Targetting::SELF;
	case 1: return Targetting::SINGLE_TARGET;
	case 2: return Targetting::AOE;
	case 3: return Targetting::LINE;
	default: return Targetting::SELF;
	}
}

static CardType decode_card_type(int id)
{
	int category = get_category_from_id(id);
	int family = get_family_from_id(id);

	switch (category)
	{
	case 1:
		switch (family)
		{
		case 0: return CardType::SLASHING;
		case 1: return CardType::PIERCING;
		case 2: return CardType::PIERCING;
		case 3: return CardType::FIRE;
		default: return CardType::COUNT;
		}

	case 2:
		switch (family)
		{
		case 0: return CardType::SHIELDING;
		case 1: return CardType::DAMAGE_REDUCTION;
		default: return CardType::COUNT;
		}

	case 3:
		switch (family)
		{
		case 0: return CardType::HEALING;
		case 1: return CardType::PP_BUFF;
		case 2: return CardType::MOV_BUFF;
		case 3: return CardType::ATK_BUFF;
		case 4: return CardType::DRAWING_CARD;
		default: return CardType::COUNT;
		}

	case 4:
		switch (family)
		{
		case 0: return CardType::FORCED_MOVEMENT;
		case 1: return CardType::FORCED_MOVEMENT;
		default: return CardType::FORCED_MOVEMENT;
		}
	}

	return CardType::SLASHING;
}

Entity create_ECS_card(JSON_CARD const& json_card)
{
	Entity id = ecs.createEntity();
	Components::Name nm{ json_card.name };
	CardTag cardTag = decode_card_tag(json_card.id);
	Components::Card_Value card_val{ json_card.value, decode_card_type(json_card.id) };
	Components::Targetting_Component targetting{ decode_targetting(json_card.id), json_card.range, json_card.aoe };
	Components::Card_Cost card_cost{ json_card.cost };
	Components::Card_ID cid{ json_card.id };
	Components::image_location cimg{ json_card.card_image };
	Components::Card_Description cardDes{ json_card.description };

	ecs.addComponent(id, cid);
	ecs.addComponent(id, cardTag);
	ecs.addComponent(id, nm);
	ecs.addComponent(id, card_val);
	ecs.addComponent(id, targetting);
	ecs.addComponent(id, card_cost);
	ecs.addComponent(id, cimg);
	ecs.addComponent(id, cardDes);

	return id;
}

void CardSystem::init_cards()
{
	std::vector<JSON_CARD> vec;
	if(parse_bible_card_data(vec, "Assets/cards/cards_bible.json") != JSON_RET::OK)
		return;

	for (JSON_CARD card : vec)
	{
		std::cout << "Name : " << card.name.c_str() << '\n';
		std::cout << "ID : " << card.id << '\n';
		std::cout << "Cost : " << card.cost << '\n';
		//std::cout << "Type : " << static_cast<int>(card.card_type) << '\n';
		std::cout << "Value : " << card.value << '\n';
		std::cout << "AOE : " << card.aoe << '\n';
		std::cout << "Description : " << card.description << '\n';
		std::cout << "Card Image : " << card.card_image << '\n' << '\n';
		Entity cardECSID = create_ECS_card(card);
		cards_map[card.name] = cardECSID;
		cards_vec.push_back(cardECSID);
	}

	if (parse_starter_decks(start_decks, "Assets/cards/cards_starterDecks.json") != JSON_RET::OK)
		return;
};

Entity CardSystem::generate_card_from_bible(std::string key)
{
	//if the base key card doesnt exist return
	if (cards_map.find(key) == cards_map.end())
		return (Entity)- 1;

	Entity id = ecs.createEntity();

	Entity bibleID = cards_map[key];

	Components::Name* ptr = ecs.getComponent<Components::Name>(bibleID);
	Components::Name nm{ ptr->value};
	CardTag cardTag = *(ecs.getComponent<CardTag>(bibleID));
	Components::Card_Value card_val{ ecs.getComponent<Components::Card_Value>(bibleID)->value,
									 ecs.getComponent<Components::Card_Value>(bibleID)->type };
	Components::Targetting_Component targetting{ ecs.getComponent<Components::Targetting_Component>(bibleID)->targetting_type,
												 ecs.getComponent<Components::Targetting_Component>(bibleID)->range,
												 ecs.getComponent<Components::Targetting_Component>(bibleID)->aoe };
	Components::Card_Cost card_cost{ ecs.getComponent<Components::Card_Cost>(bibleID)->value };

	Components::Card_ID cid{ ecs.getComponent<Components::Card_ID>(bibleID)->value };
	Components::image_location cimg{ecs.getComponent<Components::image_location>(bibleID)->location};
	Components::Card_Description cardDes{ ecs.getComponent<Components::Card_Description>(bibleID)->val };

	ecs.addComponent(id, cid);
	ecs.addComponent(id, cardTag);
	ecs.addComponent(id, nm);
	ecs.addComponent(id, card_val);
	ecs.addComponent(id, targetting);
	ecs.addComponent(id, card_cost);
	ecs.addComponent(id, cimg);
	ecs.addComponent(id, cardDes);
	return id;
}

Entity CardSystem::generate_card_from_bible(Entity bibleID)
{

	Entity id = ecs.createEntity();

	Components::Name nm{ ecs.getComponent<Components::Name>(bibleID)->value };
	CardTag cardTag = *(ecs.getComponent<CardTag>(bibleID));
	Components::Card_Value card_val{ ecs.getComponent<Components::Card_Value>(bibleID)->value,
									 ecs.getComponent<Components::Card_Value>(bibleID)->type };
	Components::Targetting_Component targetting{ ecs.getComponent<Components::Targetting_Component>(bibleID)->targetting_type,
												 ecs.getComponent<Components::Targetting_Component>(bibleID)->range,
												 ecs.getComponent<Components::Targetting_Component>(bibleID)->aoe };
	Components::Card_Cost card_cost{ ecs.getComponent<Components::Card_Cost>(bibleID)->value };

	Components::Card_ID cid{ ecs.getComponent<Components::Card_ID>(bibleID)->value };
	Components::image_location cimg{ ecs.getComponent<Components::image_location>(bibleID)->location };
	Components::Card_Description cardDes{ ecs.getComponent<Components::Card_Description>(bibleID)->val };

	ecs.addComponent(id, cid);
	ecs.addComponent(id, cardTag);
	ecs.addComponent(id, nm);
	ecs.addComponent(id, card_val);
	ecs.addComponent(id, targetting);
	ecs.addComponent(id, card_cost);
	ecs.addComponent(id, cimg);
	ecs.addComponent(id, cardDes);
	return id;
}

Entity CardSystem::get_bible_id(std::string key)
{
	//if the base key card doesnt exist return
	if (cards_map.find(key) == cards_map.end())
		return (Entity)- 1;

	return cards_map[key];
}

Entity CardSystem::getCardAtIndex(int index) const
{
	return cards_vec[index];
}

size_t CardSystem::size() const
{
	return cards_vec.size();
}