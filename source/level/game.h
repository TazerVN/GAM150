#pragma once
<<<<<<< HEAD
=======
#include "AEEngine.h"
#include "../util/GameStateManager.h"
#include "../factory/MeshFactory.h"
#include "../factory/TextureFactory.h"
#include "../ECS/gameObject.h"
#include "../UI/cardInteraction.h"
#include "../system/renderSystem.h"
#include "../system/inputSystem.h"
#include "../system/transformSystem.h"
#include "../system/TimerSystem.h"
#include "../system/CameraSystem.h"
#include "../system/particleSystem.h"
#include "../system/velocitySystem.h"
#include "../system/AnimationSystem.h"

#include "../ECS/Scene.h"
#include "../UI/UI.h"

static ECS::Registry ecs;
static MeshFactory mf{};
static InputSystem::InputManager IM;
static TextureFactory::TextureFactory TF;
static Camera::CameraSystem CS;
static RenderSystem::RenderSystem RM;
static CardSystem card_system;
static VelocitySystem::VelocitySystem VS;
static TransformSystem::TransformSystem TrS;
static TimerSystem::TimerSystem TS;
static Animation::AnimationSystem AS;

>>>>>>> d45adcc5a916e90cea8d81d20fa09862f8137ed8
void GameState_game_load();
void GameState_game_init();
void GameState_game_update();
void GameState_game_free();
void GameState_game_unload();