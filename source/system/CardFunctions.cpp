#include "CardFunctions.h"
#include "../util/json_parser.h"

Entity create_st_atk_card(ECS::Registry& ecs, const char* name, f32 atk, Components::DamageType dtype, f32 range, f32 cost)
{
	Components::Targetting targetting_type = Components::Targetting::SINGLE_TARGET;
	Entity id = ecs.createEntity();
	//default player values
	Components::CardTag cardTag = Components::CardTag::ATTACK;
	Components::Name nm{ name };

	Components::Card_Value card_val{ atk,dtype };
	Components::Targetting_Component targetting{ targetting_type,range,0.f };

	Components::Card_Cost card_cost{ cost };
	ecs.addComponent(id, cardTag);
	ecs.addComponent(id, nm);
	ecs.addComponent(id, card_val);
	ecs.addComponent(id, targetting);
	ecs.addComponent(id, card_cost);
	return id;
}

Entity create_aoe_atk_card(ECS::Registry& ecs, const char* name, f32 atk, Components::DamageType dtype, f32 range, f32 aoe, f32 cost)
{
	Components::Targetting targetting_type = Components::Targetting::AOE;
	Entity id = ecs.createEntity();
	//default player values
	Components::CardTag cardTag = Components::CardTag::ATTACK;
	Components::Name nm{ name };

	Components::Card_Value card_val{ atk,dtype };
	Components::Targetting_Component targetting{ targetting_type,range,aoe };

	Components::Card_Cost card_cost{ cost };
	ecs.addComponent(id, cardTag);
	ecs.addComponent(id, nm);
	ecs.addComponent(id, card_val);
	ecs.addComponent(id, targetting);
	ecs.addComponent(id, card_cost);
	return id;
}

void CardSystem::init_cards(ECS::Registry& ecs)
{
	std::vector<JSON_CARD> vec;
	JSON_RET ret = parse_data(vec, "../../Assets/cards/cards.json");

	cards.push_back(create_st_atk_card(ecs, "Sword Attack", 10.f, Components::DamageType::SLASHING, 1.f, 1.f));				//0
	cards.push_back(create_st_atk_card(ecs, "Fire Sword", 12.f, Components::DamageType::FIRE, 2.f, 1.f));						//1
	cards.push_back(create_st_atk_card(ecs, "Steven Sword", 20.f, Components::DamageType::PIERCING, 3.f, 1.f));				//2
	cards.push_back(create_st_atk_card(ecs, "Gun", 8.f, Components::DamageType::PIERCING, 9.f, 1.f));							//3
	cards.push_back(create_aoe_atk_card(ecs, "Black Hole", 1.f, Components::DamageType::BLUDGEONING, 4.f, 3.f, 1.f));		//4
	//cards.push_back(create_defense_card(ecs,"Shield", 10.f, 0.f));							
};
Entity& CardSystem::get_card(CardSystemNames card)
{
	return cards[static_cast<int>(card)];
}
size_t CardSystem::size() const
{
	return cards.size();
}