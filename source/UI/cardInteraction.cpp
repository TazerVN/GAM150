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

	void hand_onHover(ECS::Registry& ecs, Entity id)
	{
		Components::Transform* t = ecs.getComponent<Components::Transform>(id);
		t->pos_onscreen.y = t->pos.y + 40;
		t->size_col.x = t->size.x + 40;
	}

	void hand_offHover(ECS::Registry& ecs, Entity id)
	{
		Components::Transform* t = ecs.getComponent<Components::Transform>(id);
		t->pos_onscreen = t->pos;
		t->size_col.x = t->size.x;
	}


	CardHand::CardHand() : curr_hand_display{}, curr_card_id{}, activate{}, id{ 0 }
	{
		curr_hand_display.reserve(MAX_CARDS_HAND + 1);
		curr_card_id.reserve(MAX_CARDS_HAND + 1);
		activate.reserve(MAX_CARDS_HAND + 1);
		this->reset = true;
		tbsptr = nullptr;
		gbsptr = nullptr;
	}


	CardHand::CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height) : CardHand()
	{
		Entity id;
		this->reset = true;
		this->id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height}, {width, height},0.0f };
		Components::Input input(AEVK_SPACE, true, [this] { this->reset_hand(); }, [id = this->id, &ecs] { hand_onHover(ecs, id); }, [id = this->id, &ecs] { hand_offHover(ecs, id); });
		ecs.addComponent(this->id, input);
		ecs.addComponent(this->id, trans);
	}

	CardHand::CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, TBS::TurnBasedSystem& tbs) : CardHand()
	{
		this->reset = true;
		this->id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height}, {width, height},0.0f };
		Components::Input input(AEVK_SPACE, true, [this] { this->reset_hand(); }, [id = this->id, &ecs] { hand_onHover(ecs, id); }, [id = this->id, &ecs] { hand_offHover(ecs, id); });
		ecs.addComponent(this->id, input);
		ecs.addComponent(this->id, trans);
		tbsptr = &tbs;		//Twan i added the pointer to the turnbase for u nig
	}

	CardHand::CardHand(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, TBS::TurnBasedSystem& tbs,
		PhaseSystem::GameBoardState& gbs)
		: CardHand()
	{
		this->reset = true;
		this->id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height}, {width, height},0.0f };
		Components::Input input(AEVK_SPACE, true, nullptr, [id = this->id, &ecs] { hand_onHover(ecs, id); }, [id = this->id, &ecs] { hand_offHover(ecs, id); });
		ecs.addComponent(this->id, input);
		ecs.addComponent(this->id, trans);
		tbsptr = &tbs;
		gbsptr = &gbs;
	}

	void CardHand::update_logic(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, MeshFactory& mf, TextureFactory::TextureFactory& tf)
	{
		if (gbsptr == nullptr) return;
		if (!(gbsptr->getGBPhase() == PhaseSystem::GBPhase::MAIN_PHASE)) return;

		for (int i = 0; i < this->curr_hand_display.size(); i++)
		{
			if (this->activate[i] == true)
			{
				if (gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::CARD_SELECT)	//if not in card_select or player explore
				{
					tbsptr->select_hand_index(i);
					tbsptr->select_card(ecs);
					gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::GRID_SELECT);
				}
				//deleting the visual card code

				/*selectableCard_delete(ecs, this->curr_hand_display[i]);

				this->curr_hand_display.erase(this->curr_hand_display.begin() + i);
				this->curr_card_id.erase(this->curr_card_id.begin() + i);
				this->activate.erase(this->activate.begin() + i);*/
				this->activate[i] = false;
			}
		}


		if (this->reset == true)
		{
			std::cout << "cardhand address: " << this << std::endl;
			this->generateCards(ecs, tbs, mf, tf);
			this->reset = false;
			std::cout << "Shuffle New Cards" << std::endl;

			Components::Input* in = ecs.getComponent<Components::Input>(this->id);
			in->onClick = [this] { this->reset_hand(); };
		}
		this->update_pos(ecs);

	}

	void CardHand::update_pos(ECS::Registry& ecs)
	{

		Components::Transform* cardhand_pos = ecs.getComponent<Components::Transform>(this->id);

		int i = 0;
		for (Entity eID : this->curr_hand_display)
		{

			Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(eID);
			Components::Input* in = ecs.getComponent<Components::Input>(eID);
			Components::Transform* transform = ecs.getComponent<Components::Transform>(eID);

			transform->pos.x = cardhand_pos->pos_onscreen.x + (f32(i) / curr_hand_display.size()) * cardhand_pos->size_col.x - cardhand_pos->size_col.x / 2 + transform->size_col.x / 2;
			transform->pos.y = cardhand_pos->pos_onscreen.y;
			i++;
		}
	}

	void CardHand::generateCards(ECS::Registry& ecs, TBS::TurnBasedSystem& tbs, MeshFactory& mf, TextureFactory::TextureFactory& tf)
	{
		ECS::ComponentTypeID cID = ECS::getComponentTypeID<Components::Card_Storage>();

		if (!ecs.getBitMask()[tbs.current()].test(cID)) return;

		Components::Card_Storage* cs = ecs.getComponent<Components::Card_Storage>(tbs.current());



		for (size_t i = 0; i < cs->data_card_hand.size(); i++)
		{
			this->curr_card_id.push_back(cs->data_card_hand.at(i));
			this->activate.push_back(false);

			ECS::ComponentTypeID aID = ECS::getComponentTypeID<Components::Name>();
			if (ecs.getBitMask()[i].test(aID)) {};

			Components::Name* name = ecs.getComponent<Components::Name>(cs->data_card_hand.at(i));
			Components::Attack* a = ecs.getComponent<Components::Attack>(cs->data_card_hand.at(i));

			AEGfxTexture* texture = nullptr;

			switch (a->type)
			{
			case (Components::SLASHING):
				texture = tf.getTextureCard(TextureFactory::C_SLASH);
				break;
			case (Components::PIERCING):
				texture = tf.getTextureCard(TextureFactory::C_SLASH2);
				break;
			case (Components::FIRE):
				texture = tf.getTextureCard(TextureFactory::C_BARRIER);
				break;
			case (Components::BLUDGEONING):
			default:
				texture = tf.getTextureCard(TextureFactory::C_BLACKHOLE);
				break;
			}

			Entity eid = ecs.createEntity();

			this->curr_hand_display.push_back(selectableCard_create(eid, ecs, mf, 0, -500, 162, 264, 0, 1, texture, [this, eid] { this->activate_card(eid); }));
		}
	}

	void CardHand::activate_card(Entity e)
	{
		for (int i = 0; i < this->curr_hand_display.size(); i++)
		{
			if (this->curr_hand_display[i] == e)
			{
				this->activate[i] = true;
			}
		}
	}

	void CardHand::remove_card(ECS::Registry& ecs,size_t index)
	{
		selectableCard_delete(ecs, this->curr_hand_display[index]);

		this->curr_hand_display.erase(this->curr_hand_display.begin() + index);
		this->curr_card_id.erase(this->curr_card_id.begin() + index);
		this->activate.erase(this->activate.begin() + index);
	}

	void CardHand::reset_hand()
	{
		this->reset = true;
		std::cout << "this address: " << this << std::endl;

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

	Entity selectableCard_create(Entity id, ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> fp)
	{
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

	Entity selectableCard_create(Entity id, ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex, std::function<void()> fp)
	{
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
