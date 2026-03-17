#pragma once

#include "../types.h"
//#include "../UI/cardInteraction.h"
//#include "../ECS/Scene.h"
#include "../UI/UI.h"
#include "../UI/Text.h"
#include "../system/particleSystem.h"

#include "CombatLevel.h"
void LevelStateCombat_load();
void LevelStateCombat_init();
void LevelStateCombat_update();
void LevelStateCombat_free();
void LevelStateCombat_unload();