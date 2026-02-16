#pragma once

#include "AEEngine.h"
#include "../ECS/ECSystem.h"
#include "../factory/MeshFactory.h"
#include "../system/renderSystem.h"
#include "../system/GridSystem.h"
/*
class UI
{

};
*/

namespace UI
{
	Entity hp_bar(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
}

