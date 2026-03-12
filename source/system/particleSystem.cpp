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
				transform->pos_onscreen.y += dt * AERandFloat() * 10 ;
				//transform->pos_onscreen.x = 0.0f;
				Components::Color* color = ecs.getComponent<Components::Color>(ent);
				//color->d_color.b = 0.2f * AERandFloat();
				Components::Timer* timer = ecs.getComponent<Components::Timer>(ent);
				if (timer->start == false)
				{
					ecs.destroyEntity(ent);
				}
				
			}
		}
	}
}



//void Particle::ParticleSystem::recycle(Entity e)
//{
//}

void Particle::ParticleSystem::spawn_one(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z ,f32 velX, f32 velY)
{
	Entity id = ecs.createEntity();
	//default single particle value
	Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
	Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
	Components::Color color{ 0.4f - 0.4f * AERandFloat(), 0.7f + 0.3f * AERandFloat(), 1.0f, AERandFloat()};
	Components::Timer timer { AERandFloat() };
	Components::Particle particle { 1.0f };
	Components::Velocity vel{0.f, 0.f};

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
	int max_count = 200;
	for(int i = 0; i < max_count; i++)
	{
		f32 x = -400.0f * AERandFloat(), y = 400.0f * AERandFloat();
		

		spawn_one(ecs, mf, x, y, 50.0f, 50.0f, 0.0f, 1, 0.0f, 0.0f);
	}

}

void Particle::ParticleSystem::particleBurst(ECS::Registry& ecs, MeshFactory& mf)
{
	int max_count = 50;
	for (int i = 0; i < max_count; i++)
	{
		// Generate random velocity here, pass it in
		f32 velX = (AERandFloat() * 400.f) - 200.f;
		f32 velY = (AERandFloat() * 400.f) - 200.f;

		spawn_one(ecs, mf, 0.f, 0.f, 20.f, 20.f, 0.f, 1, velX, velY);
	}
}