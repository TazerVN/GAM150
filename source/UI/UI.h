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

#include "../UI/MenuUI.h"

#include <functional>
#include "AEEngine.h"
#include <vector>

namespace UIO
{
	struct ScreenTransition;
}

namespace UI
{
	
	class CombatUI
	{
	};

	class UIManager
	{
		private:
		s32 z{1000};

		//fade in
		UIO::ScreenTransition st;

		// ============ MENU ===============
		public:
		MenuUI menu;
		PauseMenu pause;


		// ============ COMBAT ===============
		private:
		std::vector<std::pair<Entity, Entity>> hp_children_list;
		std::vector<std::pair<Entity, Entity>> mana_children_list;
		std::vector<std::pair<Entity, Entity>> intention_children_list;
		Entity player_effect{0};
		std::vector<Entity> current_ui;

		//card
		CardInformation::CardDisplay info;
		CardInteraction::CardHand hand;
		Entity turn;
		
		Victory_Select vicSelect;

		//other
		std::pair<Entity, Entity> draw_pile;
		std::pair<Entity, Entity> discard_pile;

		void health_update();
		void stamina_update();
		void pp_update(Scene& scene);
		void turn_update(Scene& scene);
		void drawpile_update();
		void discardpile_update();
		void player_effect_update();


		public:

		UIManager();
		void update(Scene& scene, f32 dt);
		void combat_init(Scene& scene);
		void menu_init();
		void free();

		CardInteraction::CardHand& getCardHand();
		CardInformation::CardDisplay& getCardDisplay();
		PauseMenu& getPauseMenu();
		Victory_Select& getVictoryMenu();

		/*void ui_cardInformation(EntityComponent::Registry& ecs, Entity Id);
		void remove_cardInformation();*/
	};

}

