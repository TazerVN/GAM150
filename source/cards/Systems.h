#pragma once
#include "../ECS/card.h"
#include "../ECS/Components.h"

namespace System {
	void Call_AttackSystem(ECS::Registry& ecs, Entity cardID, Entity target);
	Entity create_player(ECS::Registry& ecs, f32 x, f32 y, std::string name, f32 hp);
	Entity create_atk_card(ECS::Registry& ecs, f32 x, f32 y, std::string name, f32 atk, Components::DamageType dtype);
	void add_card_player(ECS::Registry& ecs, Entity user, Entity cardID);

	class CardSystem 
	{
	private:
		std::vector<Entity> cards;
	public:
		void init_cards(ECS::Registry& ecs);
		Entity& get_card(int index);
	};
}