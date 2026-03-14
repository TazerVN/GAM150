#pragma once

#include "../factory/MeshFactory.h"
#include "../ECS/ECSystem.h"
#include "../types.h"

#include "AEEngine.h"
#include <vector>



namespace Particle
{
	class ParticleSystem
	{
	public:

		void init(EntityComponent::Registry& ecs, MeshFactory& mf, size_t poolSize = 256);

		void update(EntityComponent::Registry& ecs, f32 dt);

		void spawn_one(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, f32 r, f32 g, f32 b, f32 alpha, f32 velX, f32 velY, Components::ParticleType type);

		void particleDigitize(EntityComponent::Registry& ecs, MeshFactory& mf);
		/*
		* Use in this:
		* - When enemy dies, spawn this particle
		* - WHen draw card, spawn this particle to as if it visually creating the card
		*/
		

		void particleBurst(EntityComponent::Registry& ecs, MeshFactory& mf);
		/*
		* Use in this:
		* - player or enemy get hit spawn this.
		*/

		//void particleBullet(ECS::Registry& ecs, MeshFactory& mf);
		/*
		* Use in this:
		* - player is using ranged attack
		*/

		void particleClick(EntityComponent::Registry& ecs, MeshFactory& mf);
		/*
		* Click effect
		*/
 

		void particleDataStream(EntityComponent::Registry& ecs, MeshFactory& mf);
		/*
		* Parallax Scrolling for Background
		*/

		void particleHeal(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y);

		void particleShield(EntityComponent::Registry& ecs, MeshFactory& mf);

		void particleDamage(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y);


	private:
		std::vector<Entity> Particlebuffer;

		//Entity acquire(ECS::Registry& ecs);
		//void   recycle(Entity e);

		
	};
}

