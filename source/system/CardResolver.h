//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Zejin Kendreich Dayap Chen
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				chen.z@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================
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
        CombatNameSpace::CombatSystem& combatSystem,
        Grid::GameBoard& board,
        TBS::TurnBasedSystem& tbs,
        IntentionDisplaySystem& intent,
        Entity caster,
        Entity cardID,
        Entity target,
        AEVec2 targetPos
    );
}