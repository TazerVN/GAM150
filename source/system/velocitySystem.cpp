//===============================================================
//	AUTHOR:		Tio Chun Yi
// 
//	EMAIL:		tio.c@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================
#include "pch.h"
#include "velocitySystem.h"
#include "../ECS/Components.h"

namespace Velocity
{
	void VelocitySystem::update()
	{
		f32 dt = f32(AEFrameRateControllerGetFrameTime());

		// Get component type IDs
		EntityComponent::ComponentTypeID velocityID = EntityComponent::getComponentTypeID<Components::Velocity>();
		EntityComponent::ComponentTypeID transformID = EntityComponent::getComponentTypeID<Components::Transform>();

		// Build bitmask - only process entities that have both
		EntityComponent::ComponentBitMask objMask;
		objMask.set(velocityID);
		objMask.set(transformID);

		for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
		{
			if ((it->first & objMask) == objMask)
			{
				for (Entity ent : it->second)
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
}