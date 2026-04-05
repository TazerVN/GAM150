#pragma once

#include "AEEngine.h"

#include "../types.h"
#include "../ECS/ECSystem.h"




namespace TransformSystem
{


	class TransformSystem
	{
		public:
		TransformSystem() = default;
		/*void update_pos(ECSystem::ECS& ecs, ECSystem::Entity& e, f32 x, f32 y);
		void update_size(ECSystem::ECS& ecs, ECSystem::Entity& e, f32 width, f32 height);
		void update_rotation(ECSystem::ECS& ecs, ECSystem::Entity& e, f32 rotation);
		AEVec2 get_pos(ECSystem::ECS& ecs, ECSystem::Entity& e);
		AEVec2 get_size(ECSystem::ECS& ecs, ECSystem::Entity& e);
		f32 get_rotation(ECSystem::ECS& ecs, ECSystem::Entity& e);*/
		void update();
	};

}



