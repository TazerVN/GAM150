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
#include "../UI/Victory_Select.h"

#include <functional>
#include "AEEngine.h"
#include <vector>

namespace UI
{
	class UIManager
	{
		private:
		std::vector<std::pair<Entity, Entity>> hp_children_list;
		std::vector<std::pair<Entity, Entity>> mana_children_list;
		std::vector<std::pair<Entity, Entity>> intention_children_list;
		std::vector<Entity> current_ui;

		//card
		CardInformation::CardDisplay info;
		CardInteraction::CardHand hand;
		
		//Pause
		PauseMenu pause;
		Victory_Select vicSelect;

		void health_update();
		void stamina_update();
		void mana_update(Scene& scene);
		public:
		UIManager();
		void update(Scene& scene, f32 dt);
		void init(Scene& scene);
		void free();

		CardInteraction::CardHand& getCardHand();
		CardInformation::CardDisplay& getCardDisplay();
		PauseMenu& getPauseMenu();
		Victory_Select& getVictoryMenu();

		/*void ui_cardInformation(EntityComponent::Registry& ecs, Entity Id);
		void remove_cardInformation();*/
	};

}

