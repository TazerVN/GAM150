#pragma once

#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../factory/MeshFactory.h"
#include "AEEngine.h"
#include <vector>


namespace Particle
{
	class ParticleSystem
	{
	public:

		void init(ECS::Registry& ecs, MeshFactory& mf, size_t poolSize = 256);

		void update(ECS::Registry& ecs, f32 dt);

		void spawn_one(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);

		void spawn(ECS::Registry& ecs, MeshFactory& mf);

	private:
		std::vector<Entity> Particlebuffer;

		//Entity acquire(ECS::Registry& ecs);
		//void   recycle(Entity e);

		
	};
}

#endif