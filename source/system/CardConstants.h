#pragma once
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