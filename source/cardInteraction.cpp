#include "cardInteraction.h"
#include "ecs.h"
#include "transformSystem.h"

namespace CardInteraction
{

	CardHand::CardHand(ECSystem::ECS& ecs, MeshFactory& mf,  f32 x, f32 y, f32 width, f32 height, AEGfxTexture* pTex)
	{
		ECSystem::Entity ne = ecs.Entity_new(0);
		this->id = ne.id;
		this->z = ne.z;

		TransformSystem::TransformComponent p_transform(x, y, width, height, 0);
		ecs.ECS_add_component(*this, ECSystem::COMPONENT_TRANSFORM, &p_transform, sizeof(p_transform));
		for(s8 i = 0; i < this->arr.size(); i++){
			f32 card_x = x + float(i)/arr.size() * width*2;
			f32 card_y = y ;
			ECSystem::Entity e = selectableCard_create(ecs, mf, card_x, card_y, 162, 264, 0, 0, pTex);
			this->arr[i] = e;
		}
	}

	void CardHand::update_pos(ECSystem::ECS& ecs, TransformSystem::TransformSystem& tf,f32 x, f32 y)
	{

		tf.update_pos(ecs, *this, x, y);
		f32 width = tf.get_size(ecs, *this).x;
		for (s8 i = 0; i < this->arr.size(); i++)
		{
			f32 card_x = x + float(i) / arr.size() * width * 2;
			f32 card_y = y;
			tf.update_pos(ecs, this->arr[i], card_x, y);
		}
	}

	ECSystem::Entity selectableCard_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)
	{
		ECSystem::Entity p = ecs.Entity_new(z);

		TransformSystem::TransformComponent p_transform(x, y, width, height, 0);
		MeshComponent p_mesh(mf, MESH_RECTANGLE_CENTER, TEXTURE);
		//RenderSystem::RenderComponent p_rc(0);

		ecs.ECS_add_component(p, ECSystem::COMPONENT_TRANSFORM, &p_transform, sizeof(p_transform));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_MESH, &p_mesh, sizeof(p_mesh));

		return p;
	}

	ECSystem::Entity selectableCard_create(ECSystem::ECS& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex)
	{
		ECSystem::Entity p = ecs.Entity_new(z);

		TransformSystem::TransformComponent p_transform(x, y, width, height, 0);
		MeshComponent p_mesh(mf, MESH_RECTANGLE_CENTER, TEXTURE);
		RenderSystem::TextureComponent p_texture(pTex);

		ecs.ECS_add_component(p, ECSystem::COMPONENT_TRANSFORM, &p_transform, sizeof(p_transform));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_MESH, &p_mesh, sizeof(p_mesh));
		ecs.ECS_add_component(p, ECSystem::COMPONENT_TEXTURE, &p_texture, sizeof(p_texture));

		return p;
	}

}
