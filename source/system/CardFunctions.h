#pragma once

#include "../ECS/ECSystem.h"
#include <vector>

enum class CardSystemNames
{
	SLASH,
	SLASH_PLUS,
	SHOOT,
	SHOOT_PLUS,
	FIREBOLT,
	STEP_SLASH,
	DISK_DETONATE,
	SNIPE,
	BARRIER,
	BARRIER_PLUS,
	DMG_CUT,
	AURA_FARM,
	RECOVERY,
	RECOVERY_PLUS,
	RECOVERY_PLUS1,
	PP_UP,
	STR_DISK,
	SWIFT_FOOT,
	DATA_DRAW,
	RAND_GENERATE,
	GUST_OF_WIND,
	BLACK_HOLE
};

/*void Call_AttackSystem(ECS::Registry& ecs, Entity cardID, Entity target);*/
Entity create_st_atk_card(ECS::Registry& ecs, const char* name, f32 atk, Components::DamageType dtype, f32 range, f32 cost);
Entity create_aoe_atk_card(ECS::Registry& ecs, const char* name, f32 atk, Components::DamageType dtype, f32 range, f32 aoe, f32 cost);

class CardSystem
{
private:
	std::vector<Entity> cards;

public:
	void init_cards(ECS::Registry& ecs);
	Entity& get_card(CardSystemNames card);
	size_t size() const;
};

//attack cards 

//defense cards
//world interaction cards