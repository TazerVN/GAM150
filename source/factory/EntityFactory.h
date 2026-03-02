#pragma once
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"

#ifndef SYSTEM_H
#define SYSTEM_H



namespace System
{
	/*void Call_AttackSystem(ECS::Registry& ecs, Entity cardID, Entity target);*/
	Entity create_player(ECS::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, const char* name, f32 hp, AEGfxTexture* pTex);
	Entity create_atk_card(ECS::Registry& ecs, const char* name, f32 atk, Components::DamageType dtype, f32 range);
	Entity create_defense_card(ECS::Registry& ecs, const char* name, f32 val, f32 range);
	void add_card_player(ECS::Registry& ecs, Entity user, Entity cardID);
	void remove_card_player(ECS::Registry& ecs, Entity user, int index);

	class CardSystem
	{
	private:
		std::vector<Entity> cards;

	public:
		void init_cards(ECS::Registry& ecs);
		Entity& get_card(int index);
		size_t size() const;
	};
}
#endif // !SYSTEM_H