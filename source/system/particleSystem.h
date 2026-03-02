#pragma once

#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../factory/MeshFactory.h"
#include "AEEngine.h"
#include <vector>

namespace Component
{
	// Attached to every individual particle entity


	/*struct ParticleEmitter
	{
		AEVec2 position;
		f32	   spawnRate;
		f32	   spawnTimer;

		f32	   lifetime;
		AEVec2 velocityMin;
		AEVec2 velocityMax;
		AEVec2 sizeStart; 
		f32	   fadeSpeed;
		bool   active;

		f32 r, g, b, a;
	};*/

}

namespace Particle
{
	class ParticleSystem
	{
	public:

		void init(ECS::Registry& ecs, MeshFactory& mf, size_t poolSize = 256);

		void update(ECS::Registry& ecs, f32 dt);

		void spawn(ECS::Registry& ecs, MeshFactory& mf);

		Entity create_emitter(ECS::Registry& ecs,
			f32 x, f32 y,
			f32 spawnRate,
			f32 lifetime,
			f32 r, f32 g, f32 b, f32 a);

		void   spawn_one(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);

	private:
		std::vector<Entity> Particlebuffer;

		//Entity acquire(ECS::Registry& ecs);
		//void   recycle(Entity e);

		
	};
}

#endif