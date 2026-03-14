#pragma once

//#include "../ECS/gameObject.h"
//#include "../ECS/ECSystem.h"
//#include "../system/transformSystem.h"

#include "../types.h"

#include "../system/transformSystem.h"
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../System/TurnBasedSystem.h"
#include "../system/GridSystem.h"
#include "../system/PhaseSystem.h"
#include "../global.h"


#include <iostream>
#include "AEEngine.h"


#include "AEEngine.h"
#include "vector"
#include <utility>

#define MAX_CARDS_HAND 6


//forward declaration for gameboard state
namespace PhaseSystem { class GameBoardState; }
namespace TBS { class TurnBasedSystem; }
namespace Grid { class GameBoard; }
namespace CardInteraction
{
	void card_onHover(EntityComponent::Registry& ecs, Entity id);
	void card_offHover(EntityComponent::Registry& ecs, Entity id);
	void card_onClick(EntityComponent::Registry& ecs, Entity id);
	void card_offClick(EntityComponent::Registry& ecs, Entity id);
	class CardHand
	{
	private:
		std::vector<Entity> curr_hand_display{};
		std::vector<Entity> curr_card_id{};
		std::vector<bool> activate;
		Entity id;
		bool reset;

		TBS::TurnBasedSystem* tbsptr = nullptr;
		
		PhaseSystem::GameBoardState* gbsptr = nullptr;
		MeshFactory* mfptr = nullptr;
		Grid::GameBoard* gbptr = nullptr;
		TextureFactory::TextureFactory* tfptr = nullptr;

	public:
		CardHand(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height);
		CardHand(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, TBS::TurnBasedSystem& tbs);
		CardHand(EntityComponent::Registry& ecs, MeshFactory& mf, TextureFactory::TextureFactory& tf ,f32 x, f32 y, f32 width, f32 height, 
			TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb, PhaseSystem::GameBoardState& gbs);

		CardHand();

		void generateCards(EntityComponent::Registry& ecs, TBS::TurnBasedSystem& tbs);
		void update_logic(EntityComponent::Registry& ecs, TBS::TurnBasedSystem& tbs, MeshFactory& mf, TextureFactory::TextureFactory& tf, f32 dt);
		void update_pos(EntityComponent::Registry& ecs, f32 dt);
		void activate_card(Entity e);
		void remove_card(EntityComponent::Registry&, int);
		void reset_hand();
	};


	Entity selectableCard_create(Entity id, EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> fp);
	Entity selectableCard_create(Entity id, EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex, std::function<void()> fp);
	void selectableCard_delete(EntityComponent::Registry& ecs, Entity entity);

}
