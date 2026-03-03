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
				transform->pos_onscreen.y -= dt * AERandFloat() * 10 ;
				transform->pos_onscreen.x += 5.0f;
				Components::Color* color = ecs.getComponent<Components::Color>(ent);
				color->d_color.b = 0.2f * AERandFloat();
				Components::Timer* timer = ecs.getComponent<Components::Timer>(ent);
				if (timer->start == false)
				{
					ecs.destroyEntity(ent);
				}
				
			}
		}
	}
}




Entity Particle::ParticleSystem::create_emitter(ECS::Registry& ecs,
	f32 x, f32 y,
	f32 spawnRate,
	f32 lifetime,
	f32 r, f32 g, f32 b, f32 a)
{
	return 0;
}

//void Particle::ParticleSystem::recycle(Entity e)
//{
//}

void Particle::ParticleSystem::spawn_one(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)
{
	Entity id = ecs.createEntity();
	//default single particle value
	Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
	Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
	Components::Color color{ 0.5f, AERandFloat(), 0.5f ,AERandFloat()};
	Components::Timer timer { AERandFloat()};
	Components::Particle particle { 1.0f };
	ecs.addComponent(id, trans);
	ecs.addComponent(id, mesh);
	ecs.addComponent(id, color);
	ecs.addComponent(id, timer);
	ecs.addComponent(id, particle);

	Particlebuffer.push_back(id);

}

void Particle::ParticleSystem::spawn(ECS::Registry& ecs, MeshFactory& mf)
{
	int max_count = 200;
	for(int i = 0; i < max_count; i++)
	{
		f32 lhs = -400.0f * AERandFloat(), rhs = 400.0f * AERandFloat();
		

		spawn_one(ecs, mf, lhs, rhs, 50.0f, 50.0f, 0.0f, 1);
	}

}