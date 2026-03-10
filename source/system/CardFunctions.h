#pragma once

#include "../ECS/ECSystem.h"
#include <vector>

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

enum class Targetting
{
	SELF,
	SINGLE_TARGET,
	LINE,
	AOE
};

enum class CardTag
{
	ATTACK = 0,
	DEFENSE = 1,
	ITEM = 2,
	EVENT = 3
};

enum class DamageType
{
	SLASHING,
	PIERCING,
	BLUDGEONING,
	FIRE
};

enum class DefenseType
{
	SHIELDING,
	DAMAGE_REDUCTION,
};

enum class ItemType
{
	HEALING,
	PP_BUFF,
	ATK_BUFF,
	MOV_BUFF,
	DRAWING_CARD
};

enum class EventType
{
	FORCED_MOVEMENT
};

/*void Call_AttackSystem(ECS::Registry& ecs, Entity cardID, Entity target);*/
Entity create_st_atk_card(ECS::Registry& ecs, const char* name, f32 atk, DamageType dtype, f32 range, f32 cost);
Entity create_aoe_atk_card(ECS::Registry& ecs, const char* name, f32 atk, DamageType dtype, f32 range, f32 aoe, f32 cost);

Entity create_def_card(ECS::Registry& ecs, const char* name, f32 val, DefenseType deftype, f32 range, f32 cost);

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