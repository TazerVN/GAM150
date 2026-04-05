//=========================================
//	AUTHOR:		Wai Phyoo Ooo	
// 
//	EMAIL:		w.phyooo@digipen.edu
// 
//	DATE:		5-4-2026
//=========================================
#pragma once
#include "../types.h"

#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../factory/MeshFactory.h"
#include "../util/util.h"
#include "../util/json_parser.h"

#include <AEEngine.h>
namespace Grid { class GameBoard; }
namespace PhaseSystem { class GameBoardState; }

namespace EntityFactory
{
	Entity create_actor_spritesheet(AEVec2 pos, AEVec2 size, const char* name, f32 hp, AEGfxTexture* pTex, Components::AnimationType at);
	Entity create_actor_normal(AEVec2 pos, AEVec2 size, const char* name, f32 hp, AEGfxTexture* pTex, Components::AnimationType at);
	Entity create_grid_object(AEVec2 pos, AEVec2 size, const char* name, AEGfxTexture* pTex, f32 hp, s32 z);

	//Entity create_defense_card(ECS::Registry& ecs, const char* name, f32 val, f32 range, f32 cost);
	void add_card_player_hand(Entity user, Entity cardID);
	void add_card_player_deck(Entity user, Entity cardID);
	void remove_card_player(Entity user, int index);

	void create_player();
	void free_Player();

	class InteractableNode
	{
	private:
		Grid::GameBoard* gbptr = nullptr;
		PhaseSystem::GameBoardState* gbsptr = nullptr;
		std::vector<Entity> nodes;
	public:
		void init(Grid::GameBoard& gb, PhaseSystem::GameBoardState& gbs);
		Entity create_interactable_node(AEVec2, AEVec2, AEGfxTexture*,
			Components::AnimationType, Components::VictoryNodeTag);
		void update();
	};


}
