//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Pham Minh Tuan
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================

#include "types.h"
#include "ECS/Components.h"
#include "ECS/ECSystem.h"

#include "factory/MeshFactory.h"
#include "factory/TextureFactory.h"
#include "factory/EntityFactory.h"
#include "factory/Beastiary.h"
#include "factory/AudioFactory.h"

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


size_t playerID;
bool player_died = false;
bool pause = false;

//=====================One time declaration of extern values============================= 

EntityComponent::Registry ecs;
CardSystem card_system;
MeshFactory mf;
TextureFactory::TextureFactory TF;
Beastiary beastiary;


InputSystem::InputManager IM;
Camera::CameraSystem CS;
RenderSystem::RenderSystem RM;
Velocity::VelocitySystem VS;

Timer::TimerSystem TS;
Animation::AnimationSystem AS;
Text::PopUpText PUT;
Text::InstructionText IT;
Text::InstructionText TutorialText;
Particle::ParticleSystem PS;
AudioFactory::AudioFactory AF;
JSON_GAME_DATA gameData;
Console console;
EventPool<CommandTypes> ConsoleEvents;

std::vector<Entity> old_card_buffer;

std::string data_path_{ "Data/data.json" };
//=======================================================================================