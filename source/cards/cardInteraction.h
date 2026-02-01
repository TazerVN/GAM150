#include "../ECS/gameObject.h"
#include "../ECS/ECSystem.h"
#include "../system/transformSystem.h"
#include "AEEngine.h"
#include "array"

#ifndef CARD_H
#define CARD_H



#define MAX_CARDS_HAND 10

namespace CardInteraction
{
	class CardHand
	{
		public:
		std::array<Entity, MAX_CARDS_HAND> arr{};
		CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, AEGfxTexture* pTex);
		CardHand() = default;
		void update_pos(ECS::Registry& ecs, TransformSystem::TransformSystem& tf, f32 x, f32 y);

	};


	Entity selectableCard_create(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
	Entity selectableCard_create(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex);

}
#endif