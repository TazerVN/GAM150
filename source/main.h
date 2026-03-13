#pragma once
#include "AEEngine.h"
#include "factory/MeshFactory.h"
#include "factory/TextureFactory.h"
#include "util/GameStateManager.h"
#include "system/renderSystem.h"
#include "system/inputSystem.h"
#include "system/transformSystem.h"
#include "system/TimerSystem.h"
#include "system/CameraSystem.h"
#include "system/particleSystem.h"
#include "system/velocitySystem.h"

extern ECS::Registry ecs;
extern MeshFactory mf;
extern InputSystem::InputManager IM;
extern TextureFactory::TextureFactory TF;
extern Camera::CameraSystem CS;
extern RenderSystem::RenderSystem RM;
extern CardSystem card_system;
extern VelocitySystem::VelocitySystem VS;
extern Particle::ParticleSystem PS;
extern TransformSystem::TransformSystem TrS;
extern TimerSystem::TimerSystem TS;