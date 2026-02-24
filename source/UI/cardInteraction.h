#include "../ECS/gameObject.h"
#include "../ECS/ECSystem.h"
#include "../system/transformSystem.h"
#include "AEEngine.h"
#include "vector"

#ifndef CARD_H
#define CARD_H



#define MAX_CARDS_HAND 6

namespace CardInteraction
{
	class CardHand
	{
		private:
		std::vector<Entity> curr_hand_display{};
		std::vector<Entity> curr_card_id{};
		std::vector<bool> activate{};
		Entity id;
		bool reset;
		public:
		CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, AEGfxTexture* pTex);
		CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, AEGfxTexture* pTex, TBS::TurnBasedSystem& tbs);
		CardHand() = default;
		void update_logic(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, MeshFactory& mf, TextureFactory::TextureFactory& tf);
		void update_pos(ECS::Registry& ecs);

	};


	Entity selectableCard_create(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
	Entity selectableCard_create(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex);
	void selectableCard_delete(ECS::Registry& ecs, Entity entity);

}
#endif