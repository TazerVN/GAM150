#include "transformSystem.h"

namespace TransformSystem
{
	TransformComponent::TransformComponent(f32 x, f32 y, f32 sx, f32 sy, f32 rot)
	{
		this->pos.x = x;
		this->pos.y = y;
		this->size.x = sx;
		this->size.y = sy;
		this->rotation = rot;
		AEMtx33Identity(&this->transform);
	}

	void TransformSystem::update_pos(ECSystem::ECS& ecs, ECSystem::Entity& e, f32 x, f32 y)
	{
		TransformComponent* tc = static_cast<TransformComponent*>(ecs.ECS_get_pool(ECSystem::COMPONENT_TRANSFORM)->data[e.id]);
		tc->pos.x = x;
		tc->pos.y = y;
	}
	void TransformSystem::update_size(ECSystem::ECS& ecs, ECSystem::Entity& e, f32 width, f32 height)
	{
		TransformComponent* tc = static_cast<TransformComponent*>(ecs.ECS_get_pool(ECSystem::COMPONENT_TRANSFORM)->data[e.id]);
		tc->size.x = width;
		tc->size.y = height;
	}
	void TransformSystem::update_rotation(ECSystem::ECS& ecs, ECSystem::Entity& e, f32 x)
	{
		TransformComponent* tc = static_cast<TransformComponent*>(ecs.ECS_get_pool(ECSystem::COMPONENT_TRANSFORM)->data[e.id]);
		tc->rotation = x;
	}

	AEVec2 TransformSystem::get_pos(ECSystem::ECS& ecs, ECSystem::Entity& e)
	{
		TransformComponent* tc = static_cast<TransformComponent*>(ecs.ECS_get_pool(ECSystem::COMPONENT_TRANSFORM)->data[e.id]);
		return tc->pos;
	}
	AEVec2 TransformSystem::get_size(ECSystem::ECS& ecs, ECSystem::Entity& e)
	{
		TransformComponent* tc = static_cast<TransformComponent*>(ecs.ECS_get_pool(ECSystem::COMPONENT_TRANSFORM)->data[e.id]);
		return tc->size;
	}
	f32 TransformSystem::get_rotation(ECSystem::ECS& ecs, ECSystem::Entity& e)
	{
		TransformComponent* tc = static_cast<TransformComponent*>(ecs.ECS_get_pool(ECSystem::COMPONENT_TRANSFORM)->data[e.id]);
		return tc->rotation;
	}

}