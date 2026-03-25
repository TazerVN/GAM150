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
	Entity create_actor_normal(EntityComponent::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, const char* name, f32 hp, AEGfxTexture* pTex, Components::AnimationType at);
	Entity create_grid_object(EntityComponent::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, const char* name, AEGfxTexture* pTex, f32 hp, s32 z);

	//Entity create_defense_card(ECS::Registry& ecs, const char* name, f32 val, f32 range, f32 cost);
	void add_card_player_hand(EntityComponent::Registry& ecs, Entity user, Entity cardID);
	void add_card_player_deck(EntityComponent::Registry& ecs, Entity user, Entity cardID);
	void remove_card_player(EntityComponent::Registry& ecs, Entity user, int index);

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
		Entity create_interactable_node(EntityComponent::Registry&, MeshFactory&, AEVec2, AEVec2, AEGfxTexture*,
			Components::AnimationType, Components::VictoryNodeTag);
		void update();
		void free();
	};


}
