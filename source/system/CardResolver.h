#pragma once

#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../types.h"

#include "../system/CombatSystem.h"
#include "../system/GridSystem.h"
#include "../system/TurnBasedSystem.h"

namespace CardResolver
{
    PC_RETURN_TAG resolve(
        EntityComponent::Registry& ecs,
        CombatNameSpace::CombatSystem& combatSystem,
        Grid::GameBoard& board,
        TBS::TurnBasedSystem& tbs,
        Entity caster,
        Entity cardID,
        Entity target,
        AEVec2 targetPos
    );

    PC_RETURN_TAG resolve(
        EntityComponent::Registry& ecs,
        CombatNameSpace::CombatSystem& combatSystem,
        Grid::GameBoard& board,
        TBS::TurnBasedSystem& tbs,
        MeshFactory& mf,
        TextureFactory::TextureFactory& TF,
        Entity caster,
        Entity cardID,
        Entity target,
        AEVec2 targetPos
    );
}