#include "gameObject.h"
#include "transformSystem.h"

namespace GameObject
{
	ECSystem::Entity* gameobject_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)
	{
		ECSystem::Entity p = ecs.Entity_new(z);

		TransformSystem::TransformComponent p_transform(x, y, width, height, rotation);
		MeshComponent p_mesh(mf, MESH_RECTANGLE_CENTER, COLOR);
		//RenderSystem::RenderComponent p_rc(0);

		ecs.ECS_add_component(p, ECSystem::COMPONENT_TRANSFORM, &p_transform, sizeof(p_transform));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_MESH, &p_mesh, sizeof(p_mesh));

		return &p;
	}

	ECSystem::Entity* gameobject_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex)
	{
		ECSystem::Entity p = ecs.Entity_new(z);

		TransformSystem::TransformComponent p_transform(x, y, width, height, rotation);
		MeshComponent p_mesh(mf, MESH_RECTANGLE_CENTER, TEXTURE);
		RenderSystem::TextureComponent p_texture(pTex);

		ecs.ECS_add_component(p, ECSystem::COMPONENT_TRANSFORM, &p_transform, sizeof(p_transform));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_MESH, &p_mesh, sizeof(p_mesh));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_TEXTURE, &p_texture, sizeof(p_texture));

		return &p;
	}

	ECSystem::Entity* gameobject_grid_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)
	{
		ECSystem::Entity p = ecs.Entity_new(z);

		TransformSystem::TransformComponent p_transform(x, y, width, height, rotation);
		MeshComponent p_mesh(mf, MESH_RECTANGLE_CENTER, COLOR);
		//RenderSystem::RenderComponent p_rc(0);
		Grid::GridComponent p_g{};

		ecs.ECS_add_component(p, ECSystem::COMPONENT_TRANSFORM, &p_transform, sizeof(p_transform));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_MESH, &p_mesh, sizeof(p_mesh));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_GRID, &p_g, sizeof(p_g));

		return &p;
	}

	ECSystem::Entity* gameobject_grid_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex)
	{
		ECSystem::Entity p = ecs.Entity_new(z);

		TransformSystem::TransformComponent p_transform(x, y, width, height, rotation);
		MeshComponent p_mesh(mf, MESH_RECTANGLE_CENTER, TEXTURE);
		//RenderSystem::RenderComponent p_rc(0);
		RenderSystem::TextureComponent p_texture(pTex);
		Grid::GridComponent p_g{};

		ecs.ECS_add_component(p, ECSystem::COMPONENT_TRANSFORM, &p_transform, sizeof(p_transform));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_MESH, &p_mesh, sizeof(p_mesh));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_TEXTURE, &p_texture, sizeof(p_texture));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_GRID, &p_g, sizeof(p_g));

		return &p;
	}


	
}