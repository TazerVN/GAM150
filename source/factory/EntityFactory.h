#pragma once
#include "../ECS/ECSystem.h"

#ifndef SYSTEM_H
#define SYSTEM_H



namespace System
{
	Entity create_actor(ECS::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, const char* name, f32 hp, AEGfxTexture* pTex);
	//Entity create_defense_card(ECS::Registry& ecs, const char* name, f32 val, f32 range, f32 cost);
	void add_card_player_hand(ECS::Registry& ecs, Entity user, Entity cardID);
	void add_card_player_deck(ECS::Registry& ecs, Entity user, Entity cardID);
	void remove_card_player(ECS::Registry& ecs, Entity user, int index);
}
#endif // !SYSTEM_H