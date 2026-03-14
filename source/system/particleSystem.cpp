#include "particleSystem.h"
#include "../ECS/Components.h"

void Particle::ParticleSystem::init(ECS::Registry& ecs, MeshFactory& mf, size_t poolSize)
{
}
void Particle::ParticleSystem::update(ECS::Registry& ecs, f32 dt)
{

	ECS::ComponentTypeID transID = ECS::getComponentTypeID<Components::Particle>();
	//create bitsets
	ECS::ComponentBitMask objMask;
	objMask.set(transID);

	for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
	{
		if ((it->first & objMask) == objMask)
		{
			for (Entity ent : it->second)
			{
				if (!ecs.getBitMask()[ent].test(transID)) continue;
				Components::Transform* transform = ecs.getComponent<Components::Transform>(ent);
				//transform->pos_onscreen.y += dt * 1.0f;
				//transform->pos_onscreen.x = 0.0f;
				Components::Color* color = ecs.getComponent<Components::Color>(ent);
				//color->d_color.b = 0.2f * AERandFloat();
				Components::Timer* timer = ecs.getComponent<Components::Timer>(ent);
				Components::Particle* particle = ecs.getComponent<Components::Particle>(ent);

				switch (particle->type) {
				case Components::ParticleType::Digitalize:
					transform->pos_onscreen.y += dt * 1.0f;
					color->d_color.a = (timer->max_seconds - timer->seconds / timer->max_seconds) * color->c_color.a;
					if (timer->start == false)
					{
						ecs.destroyEntity(ent);
					}
					break;

				case Components::ParticleType::Burst:
					color->d_color.a = (timer->max_seconds - timer->seconds / timer->max_seconds) * color->c_color.a;
					if (timer->start == false)
					{
						ecs.destroyEntity(ent);
					}
					break;

				case Components::ParticleType::Click:
					if (timer->start == false)
					{
						ecs.destroyEntity(ent);
					}
					break;

				case Components::ParticleType::Datastream:
					color->d_color.a = (1.0f - (timer->seconds / timer->max_seconds)) * color->c_color.a;

					// When timer ends - it reset but dosent die
					if (timer->start == false)
					{
						f32 halfWidth  = f32(AEGfxGetWindowWidth())  * 0.5f;
						f32 halfHeight = f32(AEGfxGetWindowHeight()) * 0.5f;

						f32 newX = -halfWidth + AERandFloat() * halfWidth * 2.f;
						f32 newY = -halfHeight + AERandFloat() * halfHeight * 2.f;

						transform->pos.x = newX;
						transform->pos.y = newY;
						transform->pos_onscreen.x = newX;
						transform->pos_onscreen.y = newY;

						timer->seconds = 0.0f;
						timer->start = true;
						
					}
					break;

				case Components::ParticleType::Heal:
					color->d_color.a = (1.0f - (timer->seconds / timer->max_seconds)) * color->c_color.a;
					
					transform->pos_onscreen.x += AESin(timer->seconds * 5.f) * dt * 10.f;

					if (timer->start == false)
					{
						ecs.destroyEntity(ent);
					}
					break;

				case Components::ParticleType::Shield:
					if (timer->start == false)
					{
						ecs.destroyEntity(ent);
					}
					break;

				}

			}
		}
	}
}



//void Particle::ParticleSystem::recycle(Entity e)
//{
//}

void Particle::ParticleSystem::spawn_one(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, f32 r, f32 g, f32 b, f32 alpha, f32 velX, f32 velY, Components::ParticleType type)
{
	Entity id = ecs.createEntity();
	//default single particle value
	Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, rotation };
	Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
	Components::Color color{ r, g, b, alpha };
	Components::Timer timer{ AERandFloat() };
	Components::Particle particle{ type };
	Components::Velocity vel{ 0.f, 0.f };
	vel.vel.x = velX;
	vel.vel.y = velY;

	ecs.addComponent(id, trans);
	ecs.addComponent(id, mesh);
	ecs.addComponent(id, color);
	ecs.addComponent(id, timer);
	ecs.addComponent(id, particle);
	ecs.addComponent(id, vel);



	Particlebuffer.push_back(id);

}

void Particle::ParticleSystem::particleDigitize(ECS::Registry& ecs, MeshFactory& mf)
{
	int max_count = 100;
	for (int i = 0; i < max_count; i++)
	{
		//f32 x = -250.0f * AERandFloat(), y = 250.0f * AERandFloat(); // spread position
		f32 x = -100.0f * AERandFloat(), y = 100.0f * AERandFloat();

		f32 r = 0.4f - 0.4f * AERandFloat();

		f32 g1 = 1.0f;
		f32 g2 = 0.7f + 0.3f * AERandFloat();

		f32 b1 = 0.7f + 0.3f * AERandFloat();
		f32 b2 = 1.0f;

		f32 alpha_No = 1.0f;
		f32 alpha_Rand = AERandFloat();

		spawn_one(ecs, mf, x, y, 10.0f, 40.0f, 0.5f, 10, 0.0f, g1, b1, alpha_No, 1.0f, 1.0f, Components::ParticleType::Digitalize); // rect
		spawn_one(ecs, mf, x, y, 25.0f, 40.0f, 0.4f, 10, 0.0f, g2, b2, alpha_Rand, 1.0f, 1.0f, Components::ParticleType::Digitalize); //squar
		//spawn_one(ecs, mf, x, y, 35.0f, 50.0f, 0.4f, 10, r, g2, b2, alpha_Rand, 1.0f, 1.0f);



	}

}

void Particle::ParticleSystem::particleBurst(ECS::Registry& ecs, MeshFactory& mf)
{
	int   max_count = 500;
	f32   speed = 200.f;
	
	for (int i = 0; i < max_count; i++)
	{	
		// Distribute particle evenly in full circle
		f32 angle = (f32(i) / f32(max_count)) * 2.0f * PI;

		// Convert angle to velocity direction
		f32 velX = AECos(angle) * speed;
		f32 velY = AESin(angle) * speed;

		// random speed variation
		f32 speedVariation = 0.8f + AERandFloat() * 0.4f;
		velX *= speedVariation;
		velY *= speedVariation;


		// input
		spawn_one(ecs, mf, 0.f, 0.f, 8.f, 8.f, 0.f, 1, 1.0f, 1.0f, 1.0f, 1.0f, velX, velY, Components::ParticleType::Burst);
	}
}

void Particle::ParticleSystem::particleClick(ECS::Registry& ecs, MeshFactory& mf)
{
	int   max_count = 6;
	f32   speed = 40.0f;

	for (int i = 0; i < max_count; i++)
	{
		// Distribute particle evenly in full circle
		f32 angle = (f32(i) / f32(max_count)) * 2.0f * PI;

		f32 r = 0.4f - 0.4f * AERandFloat();

		f32 g = 0.7f + 0.3f * AERandFloat();

		f32 b = 0.8f;

		f32 a = 1.0f;

		// Convert angle to velocity direction
		f32 velX = AECos(angle) * speed;
		f32 velY = AESin(angle) * speed;

		// random speed variation
		f32 speedVariation = 0.9f + AERandFloat() * 0.2f;
		velX *= speedVariation;
		velY *= speedVariation;


		// input
		spawn_one(ecs, mf, 0.f, 0.f, 20.f, 20.f, 0.f, 1, r, g, b, a, velX, velY, Components::ParticleType::Click);
	}
}

void Particle::ParticleSystem::particleDataStream(ECS::Registry& ecs, MeshFactory& mf)
{
	// Screen dimensions
	f32 screenWidth      = f32(AEGfxGetWindowWidth());
	f32 screenHeight     = f32(AEGfxGetWindowHeight());
	f32 halfWidth		 = screenWidth  * 0.5f;
	f32 halfHeight		 = screenHeight * 0.5f;

	// Set coloumn
	int coloumnCount = 20, eachColoumn = 5;
	f32 coloumnSpacing = screenWidth / f32(coloumnCount);

	
	for (int i = 0; i < coloumnCount; i++)
	{
		f32 x = -halfWidth + (i * coloumnSpacing);

		for (int row = 0; row < eachColoumn; row++)
		{
			f32 y = -halfHeight + (AERandFloat() * screenHeight);

			f32 width = 4.0f + AERandFloat() * 6.0f;
			f32 height = 15.0f + AERandFloat() * 15.0f;

			f32 streamSpeed = 80.f + AERandFloat() * 120.0f;

			f32 velX = 0.707f * streamSpeed;
			f32 velY = 0.707f * streamSpeed;

			f32 r = 0.0f;
			f32 g = 0.7f + 0.3f * AERandFloat();
			f32 b = 0.8f + 0.2f * AERandFloat();
			f32 a = 1.0;

			//0.3f + 0.7f * AERandFloat();

			spawn_one(ecs, mf, x, y, width, height, 45.0f, -10, r, g, b, a, velX, velY, Components::ParticleType::Datastream);
		}
	}
}

void Particle::ParticleSystem::particleHeal(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y)
{
	int max_count = 15;

	for (int i = 0; i < max_count; i++)
	{
		f32 spawnX = x + (AERandFloat() * 40.0f) - 20.0f;
		f32 spawnY = y + (AERandFloat() * 20.0f) - 10.0f;

		f32 velX = (AERandFloat() * 30.f) - 15.0f;
		f32 velY = 60.0f + AERandFloat() * 40.0f;

		f32 r = 0.0f;
		f32 g = 0.7f + 0.3f * AERandFloat();
		f32 b = 0.2f * AERandFloat();
		f32 a = 0.8f + 0.2f * AERandFloat();

		spawn_one(ecs, mf, spawnX, spawnY, 8.0f, 0.8f, 0.0f, 10, r, g, b, a, velX, velY, Components::ParticleType::Heal);

	}

}

void Particle::ParticleSystem::particleShield(ECS::Registry& ecs, MeshFactory& mf)
{
}

void Particle::ParticleSystem::particleDamage(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y)
{

}
