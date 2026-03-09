#include "transformSystem.h"

namespace TransformSystem{
	void TransformSystem::update(ECS::Registry& ecs, f32& camera_x, f32& camera_y){

		ECS::ComponentTypeID transID = ECS::getComponentTypeID<Components::Particle>();
		ECS::ComponentTypeID tagID = ECS::getComponentTypeID<Components::TagClass>();
	//create bitsets
		ECS::ComponentBitMask objMask;
		objMask.set(transID);
		objMask.set(tagID);

		for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
		{
			if ((it->first & objMask) == objMask)
			{
				for (Entity ent : it->second)
				{
					if (!ecs.getBitMask()[ent].test(transID)) continue;
					Components::Transform* transform = ecs.getComponent<Components::Transform>(ent);
					Components::TagClass* tag = ecs.getComponent<Components::TagClass>(ent);
					if(tag->value == Components::Tag::CARDS || tag->value == Components::Tag::UI)
					{
						transform->pos_onscreen.x = transform->pos.x + camera_x; 
						transform->pos_onscreen.y = transform->pos.y + camera_y; 
					}

					
				}
			}
		}

	}
}