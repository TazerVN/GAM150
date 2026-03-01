#include "cardInteraction.h"
#include "../system/transformSystem.h"
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../System/TurnBasedSystem.h"
#include "../system/PhaseSystem.h"
#include <iostream>
#include "AEEngine.h"

namespace CardInteraction
{
	void fun();//forward declaration for testing
	void fu(Entity e);//forward declaration for testing
	void selectableCard_delete(ECS::Registry& ecs, Entity entity);

	CardHand::CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height)
	{
		this->reset = true;
		this->id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height}, {width, height},0.0f };
		ecs.addComponent(this->id, trans);
	}

	CardHand::CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, TBS::TurnBasedSystem& tbs)
	{
		this->reset = true;
		this->id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height}, {width, height},0.0f };
		ecs.addComponent(this->id, trans);
		tbsptr = &tbs;		//Twan i added the pointer to the turnbase for u nig
	}

	CardHand::CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, TBS::TurnBasedSystem& tbs,
		PhaseSystem::GameBoardState& gbs)
	{
		this->reset = true;
		this->id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height}, {width, height},0.0f };
		ecs.addComponent(this->id, trans);
		tbsptr = &tbs;
		gbsptr = &gbs;
	}

	void CardHand::update_logic(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, MeshFactory& mf, TextureFactory::TextureFactory& tf)
	{
		if (gbsptr == nullptr) return;

		if (gbsptr->getGBPhase() == PhaseSystem::GBPhase::MAIN_PHASE
			&& !(gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::PLAYER_EXPLORE 
			|| gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::CARD_SELECT))
			return;

		if(reset == true) 
		{
			this->generateCards(ecs, tbs, mf, tf);
			reset = false;
		}
		this->update_pos(ecs);
	}

	void CardHand::update_pos(ECS::Registry& ecs)
	{

		Components::Transform* cardhand_pos = ecs.getComponent<Components::Transform>(this->id);

		for (s8 i = 0; i < this->curr_hand_display.size(); i++)
		{
			Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(this->curr_hand_display[i]);
			Components::Input* in = ecs.getComponent<Components::Input>(this->curr_hand_display[i]);
			Components::Transform* transform = ecs.getComponent<Components::Transform>(this->curr_hand_display[i]);

			transform->pos.x = cardhand_pos->pos_onscreen.x + f32(i) / curr_hand_display.size() * cardhand_pos->size.x * 2;
			transform->pos.y = cardhand_pos->pos_onscreen.y;


			//this->curr_hand_display[i] = selectableCard_create(ecs, mf, card_x, card_y, 162, 264, 0, 1, tf.);
		}
	}

	void CardHand::generateCards(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, MeshFactory& mf, TextureFactory::TextureFactory& tf)
	{
		ECS::ComponentTypeID cID = ECS::getComponentTypeID<Components::Card_Storage>();

		if (!ecs.getBitMask()[tbs.current()].test(cID)) return;

		Components::Card_Storage* cs = ecs.getComponent<Components::Card_Storage>(tbs.current());



		for (size_t i = 0; i < cs->card_storage.size(); i++)
		{
			this->curr_card_id.push_back(cs->card_storage.at(i));

			ECS::ComponentTypeID aID = ECS::getComponentTypeID<Components::Name>();
			if (ecs.getBitMask()[i].test(aID)) {};

			Components::Name* name = ecs.getComponent<Components::Name>(cs->card_storage.at(i));


			this->curr_hand_display.push_back(selectableCard_create(ecs, mf, 0, -500, 162, 264, 0, 1, tf.getTextureCard(TextureFactory::C_SLASH), [name] { std::cout << name->value << std::endl; }));
		}
	}


	void card_onHover(ECS::Registry& ecs, Entity id)
	{
		Components::Transform* t = ecs.getComponent<Components::Transform>(id);
		Components::Color* c = ecs.getComponent<Components::Color>(id);
		c->p_color.r = 0.7f;
		c->p_color.g = 0.7f;
		c->p_color.b = 0.7f;
		t->pos_onscreen.y = t->pos.y + 20;
	}

	void card_offHover(ECS::Registry& ecs, Entity id)
	{
		Components::Transform* t = ecs.getComponent<Components::Transform>(id);
		Components::Color* c = ecs.getComponent<Components::Color>(id);
		c->p_color = c->c_color;
		t->pos_onscreen = t->pos;
	}

	Entity selectableCard_create(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> fp)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height}, {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Input input(AEVK_LBUTTON, true, fp, [id, &ecs] { card_onHover(ecs, id); }, [id, &ecs] { card_offHover(ecs, id); });
		Components::Switch s{ true };
		ecs.addComponent(id, s);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, input);

		return id;
	}

	Entity selectableCard_create(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex, std::function<void()> fp)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height}, {3 * width / 4, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Texture texture{ pTex };
		Components::Input input(AEVK_LBUTTON, true, fp, [id, &ecs] { card_onHover(ecs, id); }, [id, &ecs] { card_offHover(ecs, id); });
		Components::Switch s{ true };
		ecs.addComponent(id, s);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);
		ecs.addComponent(id, input);

		return id;
	}

	void selectableCard_delete(ECS::Registry& ecs, Entity entity)
	{
		
		ecs.destroyEntity(entity);

	}




	void fun()
	{
	}

	void fu(Entity e)
	{
	}
}
