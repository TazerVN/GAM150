#include "../ECS/gameObject.h"
#include "../ECS/ECSystem.h"
#include "../system/transformSystem.h"
#include "AEEngine.h"
#include "array"

#ifndef CARD_H
#define CARD_H



#define MAX_CARDS_HAND 6

namespace CardInteraction
{
	class CardHand
	{
		private:
		std::array<Entity, MAX_CARDS_HAND> arr{};
		std::array<bool, MAX_CARDS_HAND> activate{};
		public:
		CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, AEGfxTexture* pTex);
		CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, AEGfxTexture* pTex, TBS::TurnBasedSystem& tbs);
		CardHand() = default;
		void update(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs);

	};


	Entity selectableCard_create(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
	Entity selectableCard_create(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex);

}
#endif