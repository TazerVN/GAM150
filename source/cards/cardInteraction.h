#include "../ECS/gameObject.h"
#include "../ECS/ecs.h"
#include "../system/transformSystem.h"
#include "AEEngine.h"
#include "array"

#define MAX_CARDS 7

namespace CardInteraction
{
	class CardHand : ECSystem::Entity
	{
		public:
		std::array<ECSystem::Entity, MAX_CARDS> arr{};
		CardHand(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, AEGfxTexture* pTex);
		CardHand() = default;
		void update_pos(ECSystem::ECS& ecs, TransformSystem::TransformSystem& tf, f32 x, f32 y);
		
	};


	ECSystem::Entity selectableCard_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
	ECSystem::Entity selectableCard_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z,  AEGfxTexture* pTex);

}