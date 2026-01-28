#include "gameObject.h"

namespace GameObject
{
	ECSystem::Entity* gameobject_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation)
	{
		ECSystem::Entity p = ecs.Entity_new();

		ECSystem::TransformComponent p_transform(x, y, width, height, rotation);
		MeshComponent p_mesh(mf, MESH_RECTANGLE_CENTER, COLOR);
		RenderSystem::RenderComponent p_rc(0);

		ecs.ECS_add_component(p, ECSystem::COMPONENT_TRANSFORM, &p_transform, sizeof(p_transform));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_MESH, &p_mesh, sizeof(p_mesh));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_RENDERABLE, &p_rc, sizeof(p_rc));

		return &p;
	}

	ECSystem::Entity* gameobject_grid_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation)
	{
		ECSystem::Entity p = ecs.Entity_new();

		ECSystem::TransformComponent p_transform(x, y, width, height, rotation);
		MeshComponent p_mesh(mf, MESH_RECTANGLE_CENTER, COLOR);
		RenderSystem::RenderComponent p_rc(0);

		ecs.ECS_add_component(p, ECSystem::COMPONENT_TRANSFORM, &p_transform, sizeof(p_transform));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_MESH, &p_mesh, sizeof(p_mesh));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_RENDERABLE, &p_rc, sizeof(p_rc));

		return &p;
	}
	
}