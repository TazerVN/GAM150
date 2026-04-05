//===============================================================
//	AUTHOR		Tio Chun Yi
// 
//	EMAIL:		tio.c@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================

#include "particleSystem.h"
#include "../ECS/Components.h"
#include "global.h"


void Particle::ParticleSystem::remove(size_t index)
{
	if(Particlebuffer.empty()) return;
	if(Particlebuffer.size() > 1){
		Particlebuffer[index] = Particlebuffer[Particlebuffer.size() - 1];
		Particlebuffer.pop_back();
	}
	else
	{
		Particlebuffer.pop_back();
	}
};

void Particle::ParticleSystem::init(size_t poolSize)
{
	Particlebuffer.reserve(poolSize);
}
void Particle::ParticleSystem::update(f32 dt)
{

	EntityComponent::ComponentTypeID particleID = EntityComponent::getComponentTypeID<Components::Particle>();
	//create bitsets
	EntityComponent::ComponentBitMask objMask;
	objMask.set(particleID);


	for (int i = 0; i < this->Particlebuffer.size(); )
	{
		Entity ent = Particlebuffer[i];

		if (!ecs.getBitMask()[ent].test(particleID)) continue;
		Components::Transform* transform = ecs.getComponent<Components::Transform>(ent);
		//transform->pos_onscreen.y += dt * 1.0f;
		//transform->pos_onscreen.x = 0.0f;
		Components::Color* color = ecs.getComponent<Components::Color>(ent);
		//color->d_color.b = 0.2f * AERandFloat();
		Components::Timer* timer = ecs.getComponent<Components::Timer>(ent);
		Components::Particle* particle = ecs.getComponent<Components::Particle>(ent);

		switch (particle->type)
		{
			case Components::ParticleType::Digitalize:
				transform->pos_onscreen.y += dt * 1.0f;
				color->d_color.a = (timer->max_seconds - timer->seconds / timer->max_seconds) * color->c_color.a;
				if (timer->start == false)
				{
					ecs.destroyEntity(ent);
					this->remove(i);
					continue;
				}
				break;

			case Components::ParticleType::Burst:
				color->d_color.a = (timer->max_seconds - timer->seconds / timer->max_seconds) * color->c_color.a;
				if (timer->start == false)
				{
					ecs.destroyEntity(ent);
					this->remove(i);
					continue;
				}
				break;

			case Components::ParticleType::Click:
				if (timer->start == false)
				{
					ecs.destroyEntity(ent);
					this->remove(i);
					continue;
				}
				break;

			case Components::ParticleType::Datastream:
			{

				f32 lerp = timer->seconds / (timer->max_seconds * 0.5f) >= 1.f 
					? 1.f - timer->seconds/timer->max_seconds 
					: timer->seconds / timer->max_seconds;
				color->d_color.a = lerp * 2.f * color->c_color.a;

				// When timer ends - it reset but dosent die
				if (timer->start == false)
				{
					f32 halfWidth = f32(AEGfxGetWindowWidth()) * 0.5f;
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
			}

			case Components::ParticleType::Reversestream:
			{
				if (timer->max_seconds <= 1.0f)
				{
					timer->max_seconds = 8.0f + AERandFloat() * 4.0f;
					timer->reset = true;
				}

				transform->rotation += dt * 60.f;
				if (transform->rotation > 360.f) { transform->rotation -= 360.f; }

				f32 t = timer->seconds / timer->max_seconds;
				if (t < 0.2f)
					color->d_color.a = (t / 0.2f) * color->c_color.a;
				else
					color->d_color.a = (1.0f - t) * color->c_color.a;

				if (timer->seconds <= 0.01f && timer->start == true)
				{
					f32 halfWidth = f32(AEGfxGetWindowWidth()) * 0.5f;
					f32 halfHeight = f32(AEGfxGetWindowHeight()) * 0.5f;

					f32 newX = AERandFloat() * halfWidth;
					f32 newY = AERandFloat() * halfHeight;

					transform->pos.x = newX;
					transform->pos.y = newY;
					transform->pos_onscreen.x = newX;
					transform->pos_onscreen.y = newY;
				}
				break;
			}


			case Components::ParticleType::Heal:
				color->d_color.a = (1.0f - (timer->seconds / timer->max_seconds)) * color->c_color.a;

				transform->pos_onscreen.x += AESin(timer->seconds * 5.f) * dt * 10.f;

				if (timer->start == false)
				{
					ecs.destroyEntity(ent);
					this->remove(i);
					continue;
				}
				break;

			case Components::ParticleType::Shield:
			{
				f32 orbitSpeed = 2.0f;    // radians per second
				f32 orbitRadius = 60.f;    // must match spawn

				// Increment angle over time
				timer->seconds += dt * orbitSpeed;
				if (timer->seconds > 2.0f * PI)
					timer->seconds -= 2.0f * PI;   // wrap around

				// Recalculate position every frame
				f32 angle = timer->seconds;
				transform->pos_onscreen.x = transform->pos.x + AECos(angle) * orbitRadius;
				transform->pos_onscreen.y = transform->pos.y + AESin(angle) * orbitRadius;

				// Pulse alpha — breathing effect
				color->d_color.a = 0.5f + 0.5f * AESin(timer->seconds * 3.f);

				// Never destroyed — caller must manually free when shield breaks
				break;
			}

			case Components::ParticleType::Dataflow:
			{
				f32 t = timer->seconds / timer->max_seconds;
				color->d_color.a = (1.0f - (t * t)) * color->c_color.a;

				// Reset to left when timer ends
				if (timer->start == false)
				{
					f32 halfWidth = f32(AEGfxGetWindowWidth()) * 0.5f;

					transform->pos.x = -halfWidth;
					transform->pos.y = (AERandFloat() * 100.f) - 50.f;
					transform->pos_onscreen.x = transform->pos.x;
					transform->pos_onscreen.y = transform->pos.y;

					timer->seconds = 0.f;
					timer->start = true;
				}
				break;
			}

			case Components::ParticleType::Databub:
			{
				// Seaweed motion — sine wave on x axis
				f32 wobbleSpeed = 3.0f;   // how fast it sways
				f32 wobbleStrength = 15.f;   // how wide it sways

				transform->pos_onscreen.x += AECos(timer->seconds * wobbleSpeed) * dt * wobbleStrength;

				// Fade out near top of screen
				f32 t = timer->seconds / timer->max_seconds;
				color->d_color.a = (1.0f - (t * t)) * color->c_color.a;

				// Reset to bottom when timer ends — loops forever
				if (timer->start == false)
				{
					//f32 halfWidth = f32(AEGfxGetWindowWidth()) * 0.5f;
					f32 halfHeight = f32(AEGfxGetWindowHeight()) * 0.5f;

					// Reset near original column x, random y at bottom
					f32 newX = transform->pos.x;   // stay in same column
					f32 newY = -halfHeight + AERandFloat() * 80.f;

					transform->pos.x = newX;
					transform->pos.y = newY;
					transform->pos_onscreen.x = newX;
					transform->pos_onscreen.y = newY;

					timer->seconds = 0.f;
					timer->start = true;
				}
				break;
			}

		}


		i++;
	}
}



//void Particle::ParticleSystem::recycle(Entity e)
//{
//}

void Particle::ParticleSystem::spawn_one(bool isUI, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, f32 r, f32 g, f32 b, f32 alpha, f32 velX, f32 velY, Components::ParticleType type)
{
	Entity id = ecs.createEntity();
	//default single particle value
	Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {},  rotation };
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
	if (isUI)
	{
		Components::TagClass tag{ Components::Tag::BACKGROUND };
		ecs.addComponent(id, tag);
	}

	Particlebuffer.push_back(id);

}

//void Particle::ParticleSystem::spawn_default(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, f32 r, f32 g, f32 b, f32 alpha, f32 velX, f32 velY, Components::ParticleType type)
//{
//}

void Particle::ParticleSystem::spawn_timed(bool isUI, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, f32 r, f32 g, f32 b, f32 alpha, f32 velX, f32 velY, f32 lifetime, Components::ParticleType type)
{
	Entity id = ecs.createEntity();
	//default single particle value
	Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {},  rotation };
	Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
	Components::Color color{ r, g, b, alpha };
	Components::Timer timer{ lifetime, 0.f, true, false };
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

	if (isUI)
	{
		Components::TagClass tag{ Components::Tag::BACKGROUND };
		ecs.addComponent(id, tag);
	}

	Particlebuffer.push_back(id);

}

void Particle::ParticleSystem::particleDigitize()
{
	int max_count = 100;
	for (int i = 0; i < max_count; i++)
	{
		//f32 x = -250.0f * AERandFloat(), y = 250.0f * AERandFloat(); // spread position
		f32 x = -100.0f * AERandFloat(), y = 100.0f * AERandFloat();

		f32 g1 = 1.0f;
		f32 g2 = 0.7f + 0.3f * AERandFloat();

		f32 b1 = 0.7f + 0.3f * AERandFloat();
		f32 b2 = 1.0f;

		f32 alpha_No = 1.0f;
		f32 alpha_Rand = AERandFloat();

		spawn_one(true, x, y, 10.0f, 40.0f, 0.5f, 10, 0.0f, g1, b1, alpha_No, 1.0f, 1.0f, Components::ParticleType::Digitalize); // rect
		spawn_one(true, x, y, 25.0f, 40.0f, 0.4f, 10, 0.0f, g2, b2, alpha_Rand, 1.0f, 1.0f, Components::ParticleType::Digitalize); //squar
		//spawn_one(ecs, mf, x, y, 35.0f, 50.0f, 0.4f, 10, r, g2, b2, alpha_Rand, 1.0f, 1.0f);



	}

}

void Particle::ParticleSystem::particleBurst()
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
		spawn_one(true, 0.f, 0.f, 8.f, 8.f, 0.f, 1, 1.0f, 1.0f, 1.0f, 1.0f, velX, velY, Components::ParticleType::Burst);
	}
}

void Particle::ParticleSystem::particleClick(f32 x, f32 y)
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

		f32 a = AERandFloat();

		// Convert angle to velocity direction
		f32 velX = AECos(angle) * speed;
		f32 velY = AESin(angle) * speed;

		// random speed variation
		f32 speedVariation = 0.9f + AERandFloat() * 0.2f;
		velX *= speedVariation;
		velY *= speedVariation;


		// input
		spawn_one(true, x, y, 20.f, 20.f, 0.f, 1, r, g, b, a, velX, velY, Components::ParticleType::Click);
	}
}

void Particle::ParticleSystem::particleDataStream()
{
	// Screen dimensions
	f32 screenWidth = f32(AEGfxGetWindowWidth());
	f32 screenHeight = f32(AEGfxGetWindowHeight());
	f32 halfWidth = screenWidth * 0.5f;
	f32 halfHeight = screenHeight * 0.5f;

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
			f32 height = 50.0f + AERandFloat() * 90.0f;

			f32 streamSpeed = 80.f + AERandFloat() * 120.0f;

			f32 velX = 0.707f * streamSpeed;
			f32 velY = 0.707f * streamSpeed;

			f32 r = 0.0f;
			f32 g = 0.7f + 0.3f * AERandFloat();
			f32 b = 0.8f + 0.2f * AERandFloat();
			f32 a = 1.0;

			f32 lifetime = 2.0f + AERandFloat() * 2.0f;

			//0.3f + 0.7f * AERandFloat();

			spawn_timed(true, x, y, width, height, 125.0f, -10, r, g, b, a, velX, velY, lifetime, Components::ParticleType::Datastream);
			spawn_timed(true, x, y, width, height, 125.0f, -10, r, g, b, a, velX, velY, lifetime, Components::ParticleType::Datastream);
		}
	}
}

void Particle::ParticleSystem::particleReverseStream()
{
	// Screen dimensions
	f32 screenWidth = f32(AEGfxGetWindowWidth());
	f32 screenHeight = f32(AEGfxGetWindowHeight());
	f32 halfWidth = screenWidth * 0.5f;
	f32 halfHeight = screenHeight * 0.5f;

	int count = 3;

	for (int i = 0; i < count; i++)
	{
		// Spawn at top-right area
		f32 x = AERandFloat() * halfWidth;
		f32 y = AERandFloat() * halfHeight;

		// Direction — top-right to bottom-left
		f32 speed = 500.f + AERandFloat() * 400.f;
		f32 velX = -0.707f * speed;
		f32 velY = -0.707f * speed;

		// Square size
		f32 size = 30.f + AERandFloat() * 35.f;

		// Colour — cyan/white
		f32 r = 0.0;
		f32 g = 1.0f;
		f32 b = 0.8f + 0.2f * AERandFloat();
		f32 a = 1.0f;

		spawn_one(true, x, y, size, size, 45.f, -10, r, g, b, a, velX, velY, Components::ParticleType::Reversestream);
	}
}

void Particle::ParticleSystem::particleHeal(f32 x, f32 y, f32 w, f32 h)
{
	int max_count = 15;

	for (int i = 0; i < max_count; i++)
	{
		f32 spawnX = x + (AERandFloat() * w) - 20.0f;
		f32 spawnY = y + (AERandFloat() * h) - 10.0f;

		f32 velX = (AERandFloat() * 30.f) - 15.0f;
		f32 velY = 60.0f + AERandFloat() * 40.0f;

		f32 r = 0.0f;
		f32 g = 0.7f + 0.3f * AERandFloat();
		f32 b = 0.2f * AERandFloat();
		f32 a = 0.8f + 0.2f * AERandFloat();

		spawn_one(false, spawnX, spawnY, 8.0f, 15.0f, 0.0f, 10, r, g, b, a, velX, velY, Components::ParticleType::Heal);

	}

}

void Particle::ParticleSystem::particleShield(f32 x, f32 y, f32 r, f32 g, f32 b, f32 alpha, f32 orbitRadius, int count)
{
	for (int i = 0; i < count; i++)
	{
		// Evenly space particles around circle
		f32 angle = (f32(i) / f32(count)) * 2.0f * PI;

		// Spawn position — on the ring around entity
		f32 spawnX = x + AECos(angle) * orbitRadius;
		f32 spawnY = y + AESin(angle) * orbitRadius;

		// No velocity — orbit handled in switch case
		f32 velX = 0.f;
		f32 velY = 0.f;

		spawn_timed(false, spawnX, spawnY, 8.f, 8.f, angle, 10, r, g, b, alpha, velX, velY, 999.f, Components::ParticleType::Shield);

		Entity id = Particlebuffer.back();
		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);
		timer->seconds = angle;
	}
}



//void Particle::ParticleSystem::particleDamage(f32 x, f32 y)
//{
//
//}

void Particle::ParticleSystem::particleDataFlow()
{
	f32 screenWidth = f32(AEGfxGetWindowWidth());
	//f32 screenHeight = f32(AEGfxGetWindowHeight());
	f32 halfWidth = screenWidth * 0.5f;
	//f32 halfHeight = screenHeight * 0.5f;

	for (int i = 0; i < 50; i++)
	{
		// Spawn randomly across screen width, within horizontal band
		f32 x = -halfWidth + AERandFloat() * screenWidth;
		f32 y = (AERandFloat() * 100.f) - 50.f;   // band: -50 to +50

		// Move right only
		f32 speed = 80.f + AERandFloat() * 120.f;
		f32 velX = speed;
		f32 velY = 0.f;

		// Cyan colour
		f32 r = 0.f;
		f32 g = 0.7f + 0.3f * AERandFloat();
		f32 b = 0.8f + 0.2f * AERandFloat();
		f32 a = 0.5f + 0.5f * AERandFloat();

		spawn_one(true, x, y, 8.0f, 15.0f, 0.0f, -10, r, g, b, a, velX, velY, Components::ParticleType::Dataflow);
	}
}

void Particle::ParticleSystem::particleDataBubble()
{
	f32 screenWidth = f32(AEGfxGetWindowWidth());
	f32 halfWidth = screenWidth * 0.5f;
	f32 halfHeight = f32(AEGfxGetWindowHeight()) * 0.5f;

	int spawnPoints = 15;   // how many columns across bottom
	int perPoint = 3;    // particles per spawn point

	f32 spacing = screenWidth / f32(spawnPoints);

	for (int col = 0; col < spawnPoints; col++)
	{
		// Evenly space across bottom
		f32 x = -halfWidth + (col * spacing) + (spacing * 0.5f);

		for (int j = 0; j < perPoint; j++)
		{
			// Spawn near bottom edge
			f32 y = -halfHeight + AERandFloat() * 80.f;  // 0 to 80 above bottom

			// Upward velocity only — wobble handled in switch case
			f32 velX = 0.f;
			f32 velY = 30.f + AERandFloat() * 50.f;      // 30 to 80 upward

			// Random lifetime — creates staggered timing
			f32 lifetime = 1.5f + AERandFloat() * 3.0f;  // 1.5 to 4.5 seconds

			// Cyan/teal bubble colours
			f32 r = 0.f;
			f32 g = 0.6f + 0.4f * AERandFloat();
			f32 b = 0.7f + 0.3f * AERandFloat();
			f32 a = 0.4f + 0.6f * AERandFloat();

			// Small squares — bubble feel
			f32 size = 4.f + AERandFloat() * 8.f;

			spawn_one(true, x, y, size, size, 0.0f, -10, r, g, b, a, velX, velY, Components::ParticleType::Databub);

			// Stagger start time — so not all bubbles rise together
			Entity id = Particlebuffer.back();
			Components::Timer* timer = ecs.getComponent<Components::Timer>(id);
			timer->seconds = AERandFloat() * lifetime;   // start at random point in lifetime
		}
	}
}

void Particle::ParticleSystem::particle_system_free()
{
	EntityComponent::ComponentTypeID particleID = EntityComponent::getComponentTypeID<Components::Particle>();
	//create bitsets
	EntityComponent::ComponentBitMask objMask;
	objMask.set(particleID);


	for(Entity p : Particlebuffer)
	{	
		if (ecs.getBitMask()[p].any()) // guard against already-destroyed
			ecs.destroyEntity(p);
	}
	Particlebuffer.clear();
}
