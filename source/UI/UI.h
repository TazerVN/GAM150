#pragma once

#include "../types.h"
#include "../ECS/ECSystem.h"
#include "../factory/MeshFactory.h"
#include "../factory/TextureFactory.h"
#include "../system/renderSystem.h"
#include "../ECS/Scene.h"
#include "../UI/cardInformation.h"
#include "../UI/cardInteraction.h"
#include "../UI/PauseMenu.h"

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

		//card
		CardInformation::CardDisplay info;
		CardInteraction::CardHand hand;
		
		//Pause
		PauseMenu pause;

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
		PauseMenu& getPauseMenu();

		/*void ui_cardInformation(EntityComponent::Registry& ecs, Entity Id);
		void remove_cardInformation();*/
	};

}

