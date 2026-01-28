#include "AEEngine.h"
#include "ecs.h"
#include "Mesh_factory.h"
#include "renderSystem.h"

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H


namespace GameObject
{
	ECSystem::Entity* gameobject_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation);
	ECSystem::Entity* gameobject_grid_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation);
}
#endif // !GAMEOBJECT_H