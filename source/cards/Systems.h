#pragma once
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"

#ifndef SYSTEM_H
#define SYSTEM_H



namespace System
{
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
#endif // !SYSTEM_H