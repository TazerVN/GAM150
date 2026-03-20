#pragma once

#include "../types.h"
#include "../ECS/ECSystem.h"
#include "../factory/MeshFactory.h"
#include "../factory/TextureFactory.h"
#include "../system/renderSystem.h"
#include "../ECS/Scene.h"
#include "../UI/cardInformation.h"
#include "../UI/cardInteraction.h"

#include <functional>
#include "AEEngine.h"
#include <vector>

namespace UI
{
	class UIManager
	{
		private:
		std::vector<std::pair<Entity, Entity>> actor_children_list;
		std::vector<std::pair<Entity, Entity>> mana_children_list;
		std::vector<Entity> current_ui;
		CardInformation::CardDisplay info;
		CardInteraction::CardHand hand;

		void health_update(EntityComponent::Registry&);
		void stamina_update(EntityComponent::Registry&);
		void mana_update(Scene& scene);
		public:
		UIManager();
		void update(Scene& scene, f32 dt);
		void init(Scene& scene, MeshFactory& mf,  TextureFactory::TextureFactory& tf);
		void free(EntityComponent::Registry& ecs);

		CardInteraction::CardHand& getCardHand();
		CardInformation::CardDisplay& getCardDisplay();

		Entity ui_hp_bar(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
		Entity ui_stamina_bar(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
		Entity ui_button(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func);
		Entity ui_text(EntityComponent::Registry& ecs, MeshFactory& mf, TextureFactory::TextureFactory& tf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, const char* a);
		Entity ui_button_texture(EntityComponent::Registry& ecs, MeshFactory& mf, AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func);
		Entity ui_anchor(EntityComponent::Registry& ecs, f32 x, f32 y, f32 width, f32 height, f32 rotation);
		Entity ui_blank(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
		Entity ui_blank_texture(EntityComponent::Registry& ecs, MeshFactory& mf, AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
		/*void ui_cardInformation(EntityComponent::Registry& ecs, Entity Id);
		void remove_cardInformation();*/
	};

}

