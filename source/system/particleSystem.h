//===============================================================
//	AUTHOR:		Tio Chun Yi
// 
//	EMAIL:		tio.c@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once


#include "../ECS/ECSystem.h"
#include "../factory/MeshFactory.h"
#include "../types.h"

#include "AEEngine.h"
#include <vector>



namespace Particle
{
	class ParticleSystem
	{
	public:

		void init(size_t poolSize = 256);

		void update(f32 dt);

		void remove(size_t index);


		void spawn_one(bool isUI,f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, f32 r, f32 g, f32 b, f32 alpha, f32 velX, f32 velY, Components::ParticleType type);

		//void spawn_default(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, f32 r, f32 g, f32 b, f32 alpha, f32 velX, f32 velY, Components::ParticleType type);

		void spawn_timed(bool isUI,f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, f32 r, f32 g, f32 b, f32 alpha, f32 velX, f32 velY,f32 lifetime, Components::ParticleType type);

		void particleDigitize();
		/*
		* Use in this:
		* - When enemy dies, spawn this particle
		* - WHen draw card, spawn this particle to as if it visually creating the card
		*/
		

		void particleBurst();
		/*
		* Use in this:
		* - player or enemy get hit spawn this.
		*/

		//void particleBullet(ECS::Registry& ecs, MeshFactory& mf);
		/*
		* Use in this:
		* - player is using ranged attack
		*/

		void particleClick(f32 x, f32 y);
		/*
		* Click effect
		*/
 

		void particleDataStream();
		/*
		* Parallax Scrolling for Background
		*/

		void particleReverseStream();
		/*
		* Parallax Scrolling for Background
		*/

		void particleHeal(f32 x, f32 y, f32 w, f32 h);


		void particleShield(f32 x, f32 y, f32 r, f32 g, f32 b, f32 alpha, f32 orbitRadius, int count);

		//void particleDamage(f32 x, f32 y);

		void particleDataFlow();

		void particleDataBubble();

		void particle_system_free();

	private:
		std::vector<Entity> Particlebuffer;
	};
}

