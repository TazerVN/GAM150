#include "pch.h"

size_t playerID;
bool new_Start;

//=====================One time declaration of extern values============================= 

EntityComponent::Registry ecs;
MeshFactory mf;
TextureFactory::TextureFactory TF;


InputSystem::InputManager IM;
Camera::CameraSystem CS;
RenderSystem::RenderSystem RM;
CardSystem card_system;
Velocity::VelocitySystem VS;
TransformSystem::TransformSystem TrS;
Timer::TimerSystem TS;
Animation::AnimationSystem AS;
Text::PopUpText PUT;

//=======================================================================================