#include "pch.h"

size_t playerID = -1;
bool player_died = false;
bool pause = false;
bool first_load = true;

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
TransformSystem::TransformSystem TrS;
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