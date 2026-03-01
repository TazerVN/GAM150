#include "../ECS/gameObject.h"
#include "../ECS/ECSystem.h"
#include "../system/transformSystem.h"
#include "AEEngine.h"
#include "vector"

#ifndef CARD_H
#define CARD_H

#define MAX_CARDS_HAND 6

//forward declaration for gameboard state
namespace PhaseSystem { class GameBoardState; }
namespace TBS { class TurnBasedSystem; }

namespace CardInteraction
{
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

	public:
		CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height);
		CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, TBS::TurnBasedSystem& tbs);
		CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, TBS::TurnBasedSystem& tbs, PhaseSystem::GameBoardState& gbs);

		CardHand();

		void generateCards(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, MeshFactory& mf, TextureFactory::TextureFactory& tf);
		void update_logic(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, MeshFactory& mf, TextureFactory::TextureFactory& tf);
		void update_pos(ECS::Registry& ecs);
		void activate_card(Entity e);
		void reset_hand();

	};


	Entity selectableCard_create(Entity id, ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> fp);
	Entity selectableCard_create(Entity id, ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex, std::function<void()> fp);
	void selectableCard_delete(ECS::Registry& ecs, Entity entity);

}
#endif