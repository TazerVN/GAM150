//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Pham Minh Tuan
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once

#include "types.h"
#include "ECS/Components.h"
#include "ECS/ECSystem.h"


#include "factory/MeshFactory.h"
#include "factory/TextureFactory.h"
#include "factory/EntityFactory.h"
#include "factory/AudioFactory.h"
#include "factory/Beastiary.h"

#include "util/GameStateManager.h"

#include "system/renderSystem.h"
#include "system/inputSystem.h"
#include "system/TimerSystem.h"
#include "system/CameraSystem.h"
#include "system/velocitySystem.h"
#include "system/AnimationSystem.h"
#include "system/particleSystem.h"
#include "system/CardSystem.h"
#include "system/ConsoleCommands.h"
#include "system/ConsoleConstants.h"
#include "UI/Text.h"



#define PLAYER_MAX_HEALTH 100;

extern size_t playerID;
extern bool player_died;
extern bool pause;

extern EntityComponent::Registry ecs;
extern TextureFactory::TextureFactory TF;
extern MeshFactory mf;

extern CardSystem card_system;
extern Beastiary beastiary;
extern Timer::TimerSystem TS;
extern InputSystem::InputManager IM;
extern Camera::CameraSystem CS;
extern RenderSystem::RenderSystem RM;
extern Velocity::VelocitySystem VS;
extern Animation::AnimationSystem AS;
extern Text::PopUpText PUT;
extern Text::InstructionText IT;
extern Text::InstructionText TutorialText;
extern Particle::ParticleSystem PS;
extern AudioFactory::AudioFactory AF;
extern JSON_GAME_DATA gameData;
extern Console console;
extern EventPool<CommandTypes> ConsoleEvents;

extern std::vector<Entity> old_card_buffer;

extern std::string data_path_;
