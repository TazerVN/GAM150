#pragma once
#include "../ECS/ECSystem.h"
#include "../global.h"

namespace TBS { class TurnBasedSystem; }

enum class COMBAT_SYSTEM_RETURN_TAG
{
	INVALID = -1,
	VALID = 0,
	DIED = 1
};

COMBAT_SYSTEM_RETURN_TAG Call_AttackSystem(ECS::Registry& ecs, Entity target, f32 damage);