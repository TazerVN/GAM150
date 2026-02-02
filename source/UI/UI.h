#pragma once

#include "AEEngine.h"
#include "../ECS/ECSystem.h"
#include "../rendering/Mesh_factory.h"
#include "../rendering/renderSystem.h"
#include "../rendering/grid.h"
/*
class UI
{

};
*/

namespace UI
{
	Entity hp_bar(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
}

