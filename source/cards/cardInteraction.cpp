#include "../cards/cardInteraction.h"
#include "../system/transformSystem.h"
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "AEEngine.h"

namespace CardInteraction
{
	void fun();//forward declaration for testing

	CardHand::CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, AEGfxTexture* pTex)
	{
		Entity hand_id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height},0.0f };

		ecs.addComponent(hand_id, trans);

		for (s8 i = 0; i < MAX_CARDS_HAND; i++)
		{

			f32 card_x = x + f32(i) / arr.size() * width * 2;
			f32 card_y = y;
			this->arr[i] = selectableCard_create(ecs, mf, card_x, card_y, 162, 264, 0, 1, pTex);;
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
		Components::Transform trans{ {x,y}, {x,y} ,{width, height},0.0f };
		Components::Mesh mesh{ mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ {1.0f, 1.0f, 1.0f ,1.0f},{1.0f, 1.0f, 1.0f ,1.0f}};
		Components::Input input{AEVK_RBUTTON, true, fun, fun};
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, input);

		return id;
	}

	Entity selectableCard_create(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height},0.0f };
		Components::Mesh mesh{ mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z};
		Components::Color color{ {1.0f, 1.0f, 1.0f ,1.0f},{1.0f, 1.0f, 1.0f ,1.0f} };
		Components::Texture texture{pTex};
		Components::Input input{AEVK_LBUTTON, true, fun, fun };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);
		ecs.addComponent(id, input);

		return id;
	}

	void fun(){
		std::cout << "FUN!" << std::endl;
	}
}
