#pragma once
enum class Targetting
{
	SELF,
	SINGLE_TARGET,
	LINE,
	AOE,
	COUNT
};

enum class CardTag
{
	ATTACK = 0,
	DEFENSE = 1,
	ITEM = 2,
	EVENT = 3,
	COUNT
};

enum class CardType
{
	SLASHING,
	PIERCING,
	BLUDGEONING,
	FIRE,
	SHIELDING,
	DAMAGE_REDUCTION,
	HEALING,
	PP_BUFF,
	ATK_BUFF,
	MOV_BUFF,
	DRAWING_CARD,
	FORCED_MOVEMENT,
	COUNT
};

//enum class DamageType
//{
//	SLASHING,
//	PIERCING,
//	BLUDGEONING,
//	FIRE
//};
//
//enum class DefenseType
//{
//	SHIELDING,
//	DAMAGE_REDUCTION,
//};
//
//enum class ItemType
//{
//	HEALING,
//	PP_BUFF,
//	ATK_BUFF,
//	MOV_BUFF,
//	DRAWING_CARD
//};
//
//enum class EventType
//{
//	FORCED_MOVEMENT
//};