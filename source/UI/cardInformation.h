#pragma once

#include "../ECS/components.h"
#include "../ECS/ECSystem.h"
#include "../System/TurnBasedSystem.h"



class CardDisplay
{
	private:
	Entity id, name, description;

	public:
	CardDisplay(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, AEGfxTexture* pTex);
	CardDisplay(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, AEGfxTexture* pTex, TBS::TurnBasedSystem& tbs);
	CardDisplay() = default;
	void update(EntityComponent::Registry& ecs);

};

