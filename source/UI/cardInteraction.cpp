#include "pch.h"
#include "cardInteraction.h"
#include "global.h"

namespace CardInteraction
{
	void fun();//forward declaration for testing
	void fu(Entity e);//forward declaration for testing
	void selectableCard_delete(EntityComponent::Registry& ecs, Entity entity);

	void hand_onHover(EntityComponent::Registry& ecs, Entity id)
	{
		//Components::Transform* t = ecs.getComponent<Components::Transform>(id);
		//t->pos_onscreen.y = t->pos.y + 40;
		//t->size_col.x = t->size.x + 40;
	}

	void hand_offHover(EntityComponent::Registry& ecs, Entity id)
	{
		/*Components::Transform* t = ecs.getComponent<Components::Transform>(id);
		t->pos_onscreen = t->pos;
		t->size_col.x = t->size.x;*/
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


	CardHand::CardHand(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height) : CardHand()
	{
		Entity id;
		this->reset = true;
		this->id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height}, {width, height},0.0f };
		Components::Input input(AEVK_SPACE, true, [this] { this->reset_hand(); }, [id = this->id, &ecs] { hand_onHover(ecs, id); }, [id = this->id, &ecs] { hand_offHover(ecs, id); });
		Components::TagClass tag{ Components::Tag::CARDS };
		ecs.addComponent(this->id, tag);
		ecs.addComponent(this->id, input);
		ecs.addComponent(this->id, trans);
	}

	CardHand::CardHand(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, TBS::TurnBasedSystem& tbs) : CardHand()
	{
		this->reset = true;
		this->id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height}, {width, height},0.0f };
		Components::Input input(AEVK_SPACE, true, [this] { this->reset_hand(); }, [id = this->id, &ecs] { hand_onHover(ecs, id); }, [id = this->id, &ecs] { hand_offHover(ecs, id); });
		Components::TagClass tag{ Components::Tag::CARDS };
		ecs.addComponent(this->id, input);
		ecs.addComponent(this->id, trans);
		ecs.addComponent(this->id, tag);
		tbsptr = &tbs;		
	}

	CardHand::CardHand(EntityComponent::Registry& ecs, MeshFactory& mf, TextureFactory::TextureFactory& tf, f32 x, f32 y, f32 width, f32 height,
					   TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb, PhaseSystem::GameBoardState& gbs, UI::UIManager& UIM)
		: CardHand()
	{
		this->reset = true;
		this->id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height}, {width, height},0.0f };
		Components::Input input(AEVK_SPACE, true, nullptr, [id = this->id, &ecs] { hand_onHover(ecs, id); }, [id = this->id, &ecs] { hand_offHover(ecs, id); });
		Components::TagClass tag{ Components::Tag::CARDS };
		ecs.addComponent(this->id, input);
		ecs.addComponent(this->id, trans);
		ecs.addComponent(this->id, tag);
		tbsptr = &tbs;
		gbsptr = &gbs;
		gbptr = &gb;
		mfptr = &mf;
		tfptr = &tf;
		uimptr= &UIM;
	}

	void CardHand::update_logic(EntityComponent::Registry& ecs, TBS::TurnBasedSystem& tbs, MeshFactory& mf, TextureFactory::TextureFactory& tf, f32 dt)
	{
		//guard against gbsptr being null
		if (gbsptr == nullptr) return;
		//if not within main phase return
		if (!(gbsptr->getGBPhase() == PhaseSystem::GBPhase::MAIN_PHASE)) return;

		if (tbsptr->current() != playerID) return;

		for (int i = 0; i < this->curr_hand_display.size(); i++)
		{
			if ((gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::GRID_SELECT || gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::AOE_GRID_SELECT))
			{
				if (tbs.get_selected_cardhand_index() == i)
				{
					card_onClick(ecs, this->curr_hand_display[i]);
				}
				else
				{
					card_offClick(ecs, this->curr_hand_display[i]);
				}
			}

			if (this->activate[i] == true)
			{
				//if (!gbptr->selected_player() && gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::PLAYER_EXPLORE)	//if not in card_select or player explore
				//{
				//	tbsptr->select_hand_index(i);
				//	tbsptr->select_card(ecs);
				//}
				this->activate[i] = false;
				Entity cardID = tbsptr->draw_card(ecs, playerID, i);
				f32& card_cost = ecs.getComponent<Components::Card_Cost>(cardID)->value;
				int& player_curMana = ecs.getComponent<Components::TurnBasedStats>(playerID)->points;
				
				std::cout << "Checking for card mana. card name : " << ecs.getComponent<Components::Name>(cardID)->value << std::endl;
				std::cout << "Card Mana : " << card_cost << std::endl;

				if (card_cost > player_curMana)
				{
					std::cout << "Not enough mana!!" << std::endl;
					PUT << "Not enough mana!!";
					return;
				}
				tbsptr->select_hand_index(i);
				tbsptr->select_card(ecs);
			}


		}


		if (this->reset == true)
		{

			//clear the data
			for (int index = 0; index < curr_hand_display.size(); index++)
			{
				selectableCard_delete(ecs, this->curr_hand_display[index]);
			}
			curr_hand_display.clear();
			curr_card_id.clear();
			activate.clear();

			this->generateCards(ecs, tbs);
			this->reset = false;
			std::cout << "Shuffle New Cards" << std::endl;

			/*Components::Input* in = ecs.getComponent<Components::Input>(this->id);
			in->onClick = [this] { this->reset_hand(); };*/
		}
		this->update_pos(ecs, dt);

	}

	void CardHand::update_pos(EntityComponent::Registry& ecs, f32 dt)
	{

		Components::Transform* cardhand_t = ecs.getComponent<Components::Transform>(this->id);

		int i = 0;
		for (Entity eID : this->curr_hand_display)
		{
			int speed = 500;
			Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(eID);
			Components::Input* in = ecs.getComponent<Components::Input>(eID);
			Components::Transform* card_t = ecs.getComponent<Components::Transform>(eID);

			int max_card = 6;
			f32 container_width_ratio = curr_hand_display.size() / (f32)max_card >= 1.f ? 1.f : curr_hand_display.size() / (f32)max_card;

			cardhand_t->size.x = container_width_ratio * cardhand_t->size_og.x;

			f32 target_x = cardhand_t->pos_onscreen.x + (f32(i) / curr_hand_display.size()) * cardhand_t->size.x - cardhand_t->size.x / 2 + card_t->size_og.x/2;
			f32 target_y = cardhand_t->pos_onscreen.y;

			/*if(transform->pos.x < target_x) transform->pos.x += dt * speed;
			else transform->pos.x -= dt * speed;

			if (transform->pos.y < target_y) transform->pos.y += dt * speed;
			else transform->pos.y -= dt * speed;*/

			card_t->pos.x = target_x;
			card_t->pos.y = target_y;
			card_t->pos_onscreen = card_t->pos;


			i++;
		}
	}

	void CardHand::generateCards(EntityComponent::Registry& ecs, TBS::TurnBasedSystem& tbs)
	{
		EntityComponent::ComponentTypeID cID = EntityComponent::getComponentTypeID<Components::Card_Storage>();

		if (!ecs.getBitMask()[tbs.current()].test(cID)) return;

		Components::Card_Storage* cs = ecs.getComponent<Components::Card_Storage>(tbs.current());

		for (size_t i = 0; i < cs->data_card_hand.size(); i++)
		{
			this->curr_card_id.push_back(cs->data_card_hand.at(i));
			this->activate.push_back(false);

			EntityComponent::ComponentTypeID aID = EntityComponent::getComponentTypeID<Components::Name>();
			if (ecs.getBitMask()[i].test(aID)) {};

			Components::Name* name = ecs.getComponent<Components::Name>(cs->data_card_hand.at(i));
			Components::Card_Value* a = ecs.getComponent<Components::Card_Value>(cs->data_card_hand.at(i));

			AEGfxTexture* texture = nullptr;


			switch (a->type)
			{
				case (CardType::SLASHING):
					texture = tfptr->getTextureCard(TextureFactory::C_SLASH);
					break;
				case (CardType::PIERCING):
					texture = tfptr->getTextureCard(TextureFactory::C_SHOOT);
					break;
				case (CardType::FIRE):
					texture = tfptr->getTextureCard(TextureFactory::C_FIREBOLT);
					break;
				case (CardType::FORCED_MOVEMENT):
					texture = tfptr->getTextureCard(TextureFactory::C_BLACKHOLE);
				default:
					texture = tfptr->getTextureCard(TextureFactory::C_SAMPLE);
					break;
			}

			Entity eid = ecs.createEntity();

			this->curr_hand_display.push_back(selectableCard_create(eid, ecs, *mfptr, 0, -500, 162, 264, 0, 30, texture, 
				[this, eid]
				{ 
					this->activate_card(eid); 
				}
			));
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

	void CardHand::remove_card(EntityComponent::Registry& ecs, int index)
	{
		selectableCard_delete(ecs, this->curr_hand_display[index]);
		this->curr_hand_display.erase(this->curr_hand_display.begin() + index);
		this->curr_card_id.erase(this->curr_card_id.begin() + index);
		this->activate.erase(this->activate.begin() + index);


		/*int j = 0;
		for(int i = 0; i < this->curr_hand_display.size(); i++)
		{
			if(j != index)
			{
				this->curr_hand_display[j] = this->curr_hand_display[i];
				this->curr_card_id[j] = this->curr_card_id[i];
				this->activate[j] = this->activate[i];
				++j;
			}

		}
		this->curr_hand_display.pop_back();
		this->curr_card_id.pop_back();
		this->activate.pop_back();*/
	}

	void CardHand::reset_hand()
	{
		this->reset = true;

	}

	void CardHand::card_interaction_free()
	{
		tbsptr = nullptr;
		gbsptr = nullptr;
		mfptr = nullptr;
		gbptr = nullptr;
		tfptr = nullptr;

		for (Entity i : curr_hand_display)
		{
			ecs.destroyEntity(i);
		}
		curr_hand_display.clear();
		curr_card_id.clear();
		activate.clear();

		ecs.destroyEntity(id);

	}

	void card_onClick(EntityComponent::Registry& ecs, Entity id)
	{
		Components::Transform* t = ecs.getComponent<Components::Transform>(id);
		Components::Color* c = ecs.getComponent<Components::Color>(id);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		c->d_color.r = c->c_color.r - minimum + (1.f - minimum) * lerp;
		c->d_color.b = c->c_color.b - minimum + (1.f - minimum) * lerp;
		c->d_color.g = c->c_color.g - minimum + (1.f - minimum) * lerp;
		t->pos_onscreen.y = t->pos.y + lerp * minimum * t->size.y/6;
	}

	void card_offClick(EntityComponent::Registry& ecs, Entity id)
	{
		Components::Transform* t = ecs.getComponent<Components::Transform>(id);
		Components::Color* c = ecs.getComponent<Components::Color>(id);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);

		c->d_color = c->c_color;
		t->pos_onscreen.y = t->pos.y;

	}

	void card_onHover(EntityComponent::Registry& ecs, Entity id)
	{
		Components::Transform* t = ecs.getComponent<Components::Transform>(id);
		Components::Mesh* m = ecs.getComponent<Components::Mesh>(id);
		Components::Input* i = ecs.getComponent<Components::Input>(id);
		Components::Color* c = ecs.getComponent<Components::Color>(id);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		m->z = 31;
		i->z = 31;

		c->d_color.r = minimum + (1.f - minimum) * lerp;
		c->d_color.b = minimum + (1.f - minimum) * lerp;
		c->d_color.g = minimum + (1.f - minimum) * lerp;
		t->pos_onscreen.y = t->pos.y + minimum + (1.f - minimum) * lerp * t->size_og.y / 4;

		t->size.y = t->size_og.y * 1.2f;
		t->size.x = t->size_og.x * 4.f / 3.f * 1.2f;
	}



	void card_offHover(EntityComponent::Registry& ecs, Entity id)
	{
		Components::Transform* t = ecs.getComponent<Components::Transform>(id);
		Components::Color* c = ecs.getComponent<Components::Color>(id);
		Components::Mesh* m = ecs.getComponent<Components::Mesh>(id);
		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);

		Components::Input* i = ecs.getComponent<Components::Input>(id);

		m->z = 30;
		i->z = 30;

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		c->d_color = c->c_color;
		t->pos_onscreen.y = t->pos.y + minimum + (1.f - minimum) * lerp * t->size.y / 4;
		t->size.y = t->size_og.y;
		t->size.x = t->size_og.x * 4.f/3.f;
		timer->seconds = 0;
	}

	Entity selectableCard_create(Entity id, EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> fp)
	{
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height}, {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Input input(AEVK_LBUTTON, true, fp, [id, &ecs] { card_onHover(ecs, id); }, [id, &ecs] { card_offHover(ecs, id); }, 30);
		Components::Switch s{ true };
		Components::TagClass tag{ Components::Tag::CARDS };
		Components::Timer timer{ 0.5f, 0.f, true, true };
		ecs.addComponent(id, tag);
		ecs.addComponent(id, s);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, input);
		ecs.addComponent(id, timer);

		return id;
	}

	Entity selectableCard_create(Entity id, EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, AEGfxTexture* pTex, std::function<void()> fp)
	{
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height}, {3 * width / 4, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Texture texture{ pTex };
		Components::Input input(AEVK_LBUTTON, true, fp, [id, &ecs] { card_onHover(ecs, id); }, [id, &ecs] { card_offHover(ecs, id); }, 30);
		Components::Switch s{ true };
		Components::TagClass tag{ Components::Tag::CARDS };
		Components::Timer timer{ 0.5f, 0.f, true, true };

		ecs.addComponent(id, tag);
		ecs.addComponent(id, s);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);
		ecs.addComponent(id, input);
		ecs.addComponent(id, timer);



		return id;
	}

	void selectableCard_delete(EntityComponent::Registry& ecs, Entity entity)
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
