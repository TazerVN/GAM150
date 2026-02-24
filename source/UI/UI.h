#pragma once

#include "AEEngine.h"
#include "../ECS/ECSystem.h"
#include "../factory/MeshFactory.h"
#include "../system/renderSystem.h"
#include "../system/GridSystem.h"
#include <vector>

namespace UI
{
	class UIManager
	{
		private:
		std::vector<Entity> list;
		public:
		UIManager() = default;
		void init(ECS::Registry& ecs, MeshFactory& mf, TextureFactory::TextureFactory& tf);
		Entity ui_hp_bar(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
		Entity ui_button(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func);
		Entity ui_text(ECS::Registry& ecs, MeshFactory& mf, TextureFactory::TextureFactory& tf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, const char* text);
	};

}

