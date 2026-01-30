#include "../cards/cardInteraction.h"
#include "../ECS/ECSystem.h"
#include "../system/transformSystem.h"
#include "../ECS/Components.h"

namespace CardInteraction
{

	CardHand::CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, AEGfxTexture* pTex)
	{
		Entity hand_id = ecs.createEntity();
		Components::Transform trans{ {x,y},{width, height},0.0f };

		ecs.addComponent(hand_id, trans);

		Components::Transform p_transform{ x, y, width, height, 0 };
		for (s8 i = 0; i < this->arr.size(); i++)
		{
			Entity id = ecs.createEntity();

			f32 card_x = x + f32(i) / arr.size() * width * 2;
			f32 card_y = y;
			this->arr[i] = selectableCard_create(ecs, mf, card_x, card_y, 162, 264, 0, 0, pTex);;
		}
	}

	void CardHand::update_pos(ECS::Registry& ecs, TransformSystem::TransformSystem& tf, f32 x, f32 y)
	{

		/*tf.update_pos(ecs, *this, x, y);
		f32 width = tf.get_size(ecs, *this).x;
		for (s8 i = 0; i < this->arr.size(); i++)
		{
			f32 card_x = x + float(i) / arr.size() * width * 2;
			f32 card_y = y;
			tf.update_pos(ecs, this->arr[i], card_x, y);
		}*/
	}

	Entity selectableCard_create(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y},{width, height},0.0f };
		Components::Mesh mesh{ mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, 1 };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}

	Entity selectableCard_create(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y},{width, height},0.0f };
		Components::Mesh mesh{ mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, 1 };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Texture texture{pTex};
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);

		return id;
	}

}
