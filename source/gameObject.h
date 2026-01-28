
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include "AEEngine.h"
#include "ecs.h"
#include "Mesh_factory.h"
#include "renderSystem.h"
#include "grid.h"


namespace GameObject
{
	ECSystem::Entity* gameobject_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation);
	ECSystem::Entity* gameobject_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, AEGfxTexture* pTex);
	ECSystem::Entity* gameobject_grid_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation);
	ECSystem::Entity* gameobject_grid_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, AEGfxTexture* pTex);
}
#endif // !GAMEOBJECT_H