#include "CardFunctions.h"
#include "../util/json_parser.h"

Entity create_st_atk_card(ECS::Registry& ecs, const char* name, f32 atk, DamageType dtype, f32 range, f32 cost)
{
	Targetting targetting_type =Targetting::SINGLE_TARGET;
	Entity id = ecs.createEntity();
	//default player values
	CardTag cardTag =CardTag::ATTACK;
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

Entity create_aoe_atk_card(ECS::Registry& ecs, const char* name, f32 atk, DamageType dtype, f32 range, f32 aoe, f32 cost)
{
	Targetting targetting_type =Targetting::AOE;
	Entity id = ecs.createEntity();
	//default player values
	CardTag cardTag = CardTag::ATTACK;
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

Entity create_card(ECS::Registry& ecs, JSON_CARD const& json_card)
{
	Entity id = ecs.createEntity();
	return id;
}

void CardSystem::init_cards(ECS::Registry& ecs)
{
	std::vector<JSON_CARD> vec;
	JSON_RET ret = parse_data(vec, "../../Assets/cards/cards.json");

	cards.push_back(create_st_atk_card(ecs, "Sword Attack", 10.f, DamageType::SLASHING, 1.f, 1.f));				//0
	cards.push_back(create_st_atk_card(ecs, "Fire Sword", 12.f, DamageType::FIRE, 2.f, 1.f));						//1
	cards.push_back(create_st_atk_card(ecs, "Steven Sword", 20.f,DamageType::PIERCING, 3.f, 1.f));				//2
	cards.push_back(create_st_atk_card(ecs, "Gun", 8.f, DamageType::PIERCING, 9.f, 1.f));							//3
	cards.push_back(create_aoe_atk_card(ecs, "Black Hole", 1.f, DamageType::BLUDGEONING, 4.f, 3.f, 1.f));		//4
	//cards.push_back(create_defense_card(ecs,"Shield", 10.f, 0.f));							

	/*for (JSON_CARD card : vec)
	{
		cards.push_back(create_card(ecs, card));
	}*/
};
Entity& CardSystem::get_card(CardSystemNames card)
{
	return cards[static_cast<int>(card)];
}
size_t CardSystem::size() const
{
	return cards.size();
}