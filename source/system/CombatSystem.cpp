#include "CombatSystem.h"
#include "../system/TurnBasedSystem.h"

COMBAT_SYSTEM_RETURN_TAG Call_AttackSystem(ECS::Registry& ecs, Entity target, f32 damage)
{
	ECS::ComponentTypeID hpID = ECS::getComponentTypeID<Components::HP>();
	//test if card have attack id
	if (!(ecs.getBitMask()[target].test(hpID))) return COMBAT_SYSTEM_RETURN_TAG::INVALID;

	//if the have components then reduce the HP amount
	ecs.getComponent<Components::HP>(target)->c_value -= damage;
	if (ecs.getComponent<Components::HP>(target)->c_value <= 0.f)
	{
		return COMBAT_SYSTEM_RETURN_TAG::DIED;
	}
	return COMBAT_SYSTEM_RETURN_TAG::VALID;
}