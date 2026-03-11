#include "CardSystem.h"
#include "../util/json_parser.h"
#include "../system/CombatSystem.h"

//Entity create_st_atk_card(ECS::Registry& ecs, const char* name, f32 atk, CardType dtype, f32 range, f32 cost)
//{
//	Targetting targetting_type =Targetting::SINGLE_TARGET;
//	Entity id = ecs.createEntity();
//	//default player values
//	CardTag cardTag =CardTag::ATTACK;
//	Components::Name nm{ name };
//
//	Components::Card_Value card_val{ atk,dtype };
//	Components::Targetting_Component targetting{ targetting_type,range,0.f };
//	Components::Card_Cost card_cost{ cost };
//
//	ecs.addComponent(id, cardTag);
//	ecs.addComponent(id, nm);
//	ecs.addComponent(id, card_val);
//	ecs.addComponent(id, targetting);
//	ecs.addComponent(id, card_cost);
//	return id;
//}
//
//Entity create_aoe_atk_card(ECS::Registry& ecs, const char* name, f32 atk, CardType dtype, f32 range, f32 aoe, f32 cost)
//{
//	Targetting targetting_type =Targetting::AOE;
//	Entity id = ecs.createEntity();
//	//default player values
//	CardTag cardTag = CardTag::ATTACK;
//	Components::Name nm{ name };
//
//	Components::Card_Value card_val{ atk,dtype };
//	Components::Targetting_Component targetting{ targetting_type,range,aoe };
//	Components::Card_Cost card_cost{ cost };
//
//	ecs.addComponent(id, cardTag);
//	ecs.addComponent(id, nm);
//	ecs.addComponent(id, card_val);
//	ecs.addComponent(id, targetting);
//	ecs.addComponent(id, card_cost);
//	return id;
//}

Entity create_card(ECS::Registry& ecs, JSON_CARD const& json_card)
{
	Entity id = ecs.createEntity();
	Components::Name nm{ json_card.name };
	CardTag cardTag = json_card.card_tag;
	Components::Card_Value card_val{ json_card.value,json_card.card_type };
	Components::Targetting_Component targetting{ json_card.targetting,json_card.range,json_card.aoe};
	Components::Card_Cost card_cost{ json_card.cost };
	
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

	for (JSON_CARD card : vec)
	{
		std::cout << "Name : " << card.name.c_str() << std::endl;
		std::cout << "Cost : " << card.cost << std::endl;
		std::cout << "Targetting : " << static_cast<int>(card.targetting) << std::endl;
		std::cout << "Card Tag : " << static_cast<int>(card.card_tag) << std::endl;
		std::cout << "Type : " << static_cast<int>(card.card_type) << std::endl;
		std::cout << "Value : " << card.value << std::endl;
		std::cout << "AOE : " << card.aoe << '\n' << std::endl;

		Entity cardECSID = create_card(ecs, card);
		cards.push_back(cardECSID);

		switch (static_cast<CardSystemNames>(card.id))
		{
		case CardSystemNames::SLASH:
		{
			cardScriptManager.add_Function
			(
				CardSystemNames::SLASH, cardECSID, 
				[&ecs,cardECSID](Entity target)
				{
					ECS::ComponentTypeID card_value_ID = ECS::getComponentTypeID<Components::Card_Value>();
					if (!ecs.getBitMask()[cardECSID].test(card_value_ID))
					{
						std::cout << "Selected card doesn't have card_data component";
						return;
						//return PC_RETURN_TAG::INVALID;
					}
					f32 card_damage = ecs.getComponent<Components::Card_Value>(cardECSID)->value;

					/*if (Call_AttackSystem(ecs, target, card_damage) == COMBAT_SYSTEM_RETURN_TAG::DIED)
					{

					}*/
				}
			);
			break;
		}
		default:
			break;
		}
	}
};
Entity& CardSystem::get_card(CardSystemNames card)
{
	return cards[static_cast<int>(card)];
}

CardScriptsManager& CardSystem::CardScripts()
{
	return cardScriptManager;
}

size_t CardSystem::size() const
{
	return cards.size();
}

void CardScriptsManager::add_Function(CardSystemNames cardName, Entity cardID,std::function<void(Entity)> function)
{
	functions[cardName] = function;
}
CardScriptReturn CardScriptsManager::runCardFunction(CardSystemNames card,Entity target)
{
	auto it = functions.find(card);
	if (it != functions.end())
	{
		it->second(target);
		return CardScriptReturn::FunctionRunSuccess;
	}
	return CardScriptReturn::FunctionNotFound;
}