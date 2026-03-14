#pragma once

#include "../types.h"
#include "AEEngine.h"
#include "../ECS/ECSystem.h"
#include "../factory/MeshFactory.h"
#include "../factory/TextureFactory.h"
#include "../system/renderSystem.h"
#include "../system/GridSystem.h"
#include "../ECS/Scene.h"

#include <functional>
#include <vector>

namespace UI
{
	class UIManager
	{
		private:
		std::vector<std::pair<Entity, Entity>> list;
		void health_update(EntityComponent::Registry& scene);
		public:
		UIManager() = default;
		void update(Scene& scene);
		void init(Scene& scene, MeshFactory& mf,  TextureFactory::TextureFactory& tf);
		Entity ui_hp_bar(Scene& scene, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
		Entity ui_button(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func);
		Entity ui_text(Scene& scene, TextureFactory::TextureFactory& tf, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, const char* a);
		Entity ui_button_texture(EntityComponent::Registry& ecs, MeshFactory& mf, TextureFactory::TextureFactory& tf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func);
	};

}

