#include "velocitySystem.h"
#include "../ECS/Components.h"

namespace VelocitySystem
{
	void VelocitySystem::update(ECS::Registry& ecs)
	{
		f32 dt = f32(AEFrameRateControllerGetFrameTime());

		// Get component type IDs
		ECS::ComponentTypeID velocityID = ECS::getComponentTypeID<Components::Velocity>();
		ECS::ComponentTypeID transformID = ECS::getComponentTypeID<Components::Transform>();

		// Build bitmask - only process entities that have both
		ECS::ComponentBitMask objMask;
		objMask.set(velocityID);
		objMask.set(transformID);

		for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
		{
			if ((it->first & objMask) == objMask)
			{
				Components::Transform* trans = ecs.getComponent<Components::Transform>(ent);
				Components::Velocity* vel = ecs.getComponent<Components::Velocity>(ent);

				if (!trans || !vel) continue;

				// Move position by velocity
				trans->pos.x += vel->vel.x * dt;
				trans->pos.y += vel->vel.y * dt;
				trans->pos_onscreen = trans->pos;
			}
		}
	}
}