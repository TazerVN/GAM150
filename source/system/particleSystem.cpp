#include "particleSystem.h"
#include "../ECS/Components.h"

void Particle::ParticleSystem::init(ECS::Registry& ecs, MeshFactory& mf, size_t poolSize)
{
}
void Particle::ParticleSystem::update(ECS::Registry& ecs, f32 dt)
{
}

void Particle::ParticleSystem::spawn(ECS::Registry& ecs, MeshFactory& mf)
{
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
	//default player values
	Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
	Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
	Components::Color color{ 0.5f, 1.0f, 0.5f ,1.0f };
	Components::Timer timer { 1.0f };
	ecs.addComponent(id, trans);
	ecs.addComponent(id, mesh);
	ecs.addComponent(id, color);
	ecs.addComponent(id, timer);

	Particlebuffer.push_back(id);

}