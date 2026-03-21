#include "pch.h"

size_t playerID;
bool new_Start = true;

//=====================One time declaration of extern values============================= 

EntityComponent::Registry ecs;
CardSystem card_system;
MeshFactory mf;
TextureFactory::TextureFactory TF;


InputSystem::InputManager IM;
Camera::CameraSystem CS;
RenderSystem::RenderSystem RM;
Velocity::VelocitySystem VS;
TransformSystem::TransformSystem TrS;
Timer::TimerSystem TS;
Animation::AnimationSystem AS;
Text::PopUpText PUT;
Particle::ParticleSystem PS;

//=======================================================================================