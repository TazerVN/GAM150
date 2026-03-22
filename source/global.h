#pragma once

#include "types.h"
#include "ECS/Components.h"
#include "ECS/ECSystem.h"

#include "factory/MeshFactory.h"
#include "factory/TextureFactory.h"
#include "factory/EntityFactory.h"

#include "util/GameStateManager.h"

#include "system/renderSystem.h"
#include "system/inputSystem.h"
#include "system/transformSystem.h"
#include "system/TimerSystem.h"
#include "system/CameraSystem.h"
#include "system/velocitySystem.h"
#include "system/AnimationSystem.h"
#include "system/particleSystem.h"
#include "system/CardSystem.h"
#include "UI/Text.h"


#define PLAYER_MAX_HEALTH 100;

extern size_t playerID;
extern bool new_Start;


extern EntityComponent::Registry ecs;
extern TextureFactory::TextureFactory TF;
extern MeshFactory mf;


extern CardSystem card_system;
extern Timer::TimerSystem TS;
extern InputSystem::InputManager IM;
extern Camera::CameraSystem CS;
extern RenderSystem::RenderSystem RM;
extern Velocity::VelocitySystem VS;
extern TransformSystem::TransformSystem TrS;
extern Animation::AnimationSystem AS;
extern Text::PopUpText PUT;
extern Particle::ParticleSystem PS;
extern AudioFactory::AudioFactory AF;
