#pragma once
#include "AEEngine.h"
#include <array>
#include <string>

#define MAX_CARDS 1000
#define MAX_COMPONENT 10

constexpr size_t NULL_INDEX = -1;
constexpr size_t MAX_HAND = 6;

namespace Components
{
	enum DamageType {
		SLASHING,
		PIERCING,
		BLUDGEONING,
		FIRE
	};
	struct Transform {
		AEVec2 pos{ 0.f,0.f };
		float rotation{ 0.f };
	};
	struct Attack {
		f32 damage;
		DamageType type;
	};
	struct Name {
		std::string value;
	};
	struct HP {
		f32 value;
	};
	struct Card_Storage
	{
		std::array<size_t, MAX_HAND> card_storage = {};
	};
};