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
		std::vector<std::pair<Entity, Entity>> actor_children_list;
		std::vector<std::pair<Entity, Entity>> children_list;
		std::vector<Entity> mana_blocks;
		void health_update(EntityComponent::Registry&);
		void stamina_update(EntityComponent::Registry&);
		void mana_update(Scene& scene);
		public:
		UIManager() = default;
		void update(Scene& scene);
		void init(Scene& scene, MeshFactory& mf,  TextureFactory::TextureFactory& tf);
		Entity ui_hp_bar(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
		Entity ui_stamina_bar(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
		Entity ui_button(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func);
		Entity ui_text(EntityComponent::Registry& ecs, MeshFactory& mf, TextureFactory::TextureFactory& tf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, const char* a);
		Entity ui_button_texture(EntityComponent::Registry& ecs, MeshFactory& mf, AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func);
		Entity ui_anchor(EntityComponent::Registry& ecs, f32 x, f32 y, f32 width, f32 height, f32 rotation);
		Entity ui_blank(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
		Entity ui_blank_texture(EntityComponent::Registry& ecs, MeshFactory& mf, AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
	};

}

