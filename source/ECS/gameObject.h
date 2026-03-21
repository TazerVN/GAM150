
#pragma once

#include "../types.h"
#include "AEEngine.h"
#include "../factory/MeshFactory.h"




namespace GameObject
{
	Entity gameobject_create(f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
	Entity gameobject_create(f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex);
	//ECSystem::Entity* gameobject_grid_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
	//ECSystem::Entity* gameobject_grid_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex);
}
