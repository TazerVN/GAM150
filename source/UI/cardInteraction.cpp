#include "pch.h"
#include "cardInteraction.h"
#include "global.h"
#include "cardInformation.h"

namespace CardInteraction
{
	void fun();//forward declaration for testing
	void fu(Entity e);//forward declaration for testing
	void selectableCard_delete(EntityComponent::Registry& ecs, std::pair<Entity,Entity> entity);
	Entity selectableCard_mana(EntityComponent::Registry& ecs, f32 pos_x, f32 pos_y, f32 width, f32 height, s32 cost, s32 z);

	void hand_onHover(Entity id)
	{
		//Components::Transform* t = ecs.getComponent<Components::Transform>(id);
		//t->pos_onscreen.y = t->pos.y + 40;
		//t->size_col.x = t->size.x + 40;
	}

	void hand_offHover(CardInformation::CardDisplay& cd, Entity id)
	{
		/*Components::Transform* t = ecs.getComponent<Components::Transform>(id);
		t->pos_onscreen = t->pos;
		t->size_col.x = t->size.x;*/
		cd.setStateOn(false);
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


	CardHand::CardHand(f32 x, f32 y, f32 width, f32 height) : CardHand()
	{
		Entity id;
		this->reset = true;
		this->id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height}, {width, height},0.0f };
		Components::Input input(AEVK_SPACE, true, nullptr, nullptr, nullptr);
		Components::TagClass tag{ Components::Tag::CARDS };
		ecs.addComponent(this->id, tag);
		ecs.addComponent(this->id, input);
		ecs.addComponent(this->id, trans);
	}

	CardHand::CardHand(f32 x, f32 y, f32 width, f32 height, TBS::TurnBasedSystem& tbs) : CardHand()
	{
		this->reset = true;
		this->id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height}, {width, height},0.0f };
		Components::Input input(AEVK_SPACE, true, nullptr, nullptr, nullptr);
		Components::TagClass tag{ Components::Tag::CARDS };
		ecs.addComponent(this->id, input);
		ecs.addComponent(this->id, trans);
		ecs.addComponent(this->id, tag);
		tbsptr = &tbs;		
	}

	CardHand::CardHand(f32 x, f32 y, f32 width, f32 height,
					   TBS::TurnBasedSystem& tbs, Grid::GameBoard& gb, 
						PhaseSystem::GameBoardState& gbs, CardInformation::CardDisplay& cd,
						CombatNameSpace::CombatSystem& cbs)
		: CardHand()
	{
		this->reset = true;
		this->id = ecs.createEntity();
		Components::Transform trans{ {x,y},{x,y},{width, height}, {width, height},0.0f };
		Components::Input input(AEVK_SPACE, true, nullptr, [id = this->id] { hand_onHover(id); }, [id = this->id, &cd] { hand_offHover(cd, id); });
		Components::TagClass tag{ Components::Tag::CARDS };
		ecs.addComponent(this->id, input);
		ecs.addComponent(this->id, trans);
		ecs.addComponent(this->id, tag);
		tbsptr = &tbs;
		gbsptr = &gbs;
		gbptr = &gb;
		mfptr = &mf;
		tfptr = &TF;
		cdptr = &cd;
		cbsptr = &cbs;
	}

	void CardHand::update_logic(f32 dt)
	{
		//guard against gbsptr being null
		this->update_pos(ecs, dt);
		if (gbsptr == nullptr) return;
		//if not within main phase return
		if (gbsptr->getGBPhase() != PhaseSystem::GBPhase::MAIN_PHASE) return;
		if(gbsptr->getPlayerPhase() != PhaseSystem::PlayerPhase::PLAYER_EXPLORE) 
			return;
		if (tbsptr->current() != playerID) return;

		

		for (int i = 0; i < this->curr_hand_display.size(); i++)
		{
			if ((gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::GRID_SELECT || gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::AOE_GRID_SELECT))
			{
				if (tbsptr->get_selected_cardhand_index() == i)
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
				this->activate[i] = false;

				Entity cardID = cbsptr->draw_card(playerID, i);
				f32& card_cost = ecs.getComponent<Components::Card_Cost>(cardID)->value;
				int& player_curMana = ecs.getComponent<Components::TurnBasedStats>(playerID)->points;
				
				std::cout << "Checking for card mana. card name : " << ecs.getComponent<Components::Name>(cardID)->value << '\n';
				std::cout << "Card Mana : " << card_cost << '\n';

				if (card_cost > player_curMana)
				{
					std::cout << "Not enough mana!!" << '\n';
					PUT << 0 << "Not enough mana!!";
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

			this->generateCards();
			this->reset = false;
			std::cout << "Shuffle New Cards" << '\n';

			/*Components::Input* in = ecs.getComponent<Components::Input>(this->id);
			in->onClick = [this] { this->reset_hand(); };*/
		}

	}

	void CardHand::update_pos(EntityComponent::Registry& ecs, f32 dt)
	{

		Components::Transform* cardhand_t = ecs.getComponent<Components::Transform>(this->id);

		int i = 0;
		int max_card = 6;
		f32 container_width_ratio = curr_hand_display.size() / (f32)max_card >= 1.f ? 1.f : curr_hand_display.size() / (f32)max_card;

		for (std::pair<Entity, Entity> eID : this->curr_hand_display)
		{
			Components::Mesh* mesh_c = ecs.getComponent<Components::Mesh>(eID.first);
			Components::Input* in_c = ecs.getComponent<Components::Input>(eID.first);
			Components::Transform* card_t_c = ecs.getComponent<Components::Transform>(eID.first);

			Components::Text* text_m = ecs.getComponent<Components::Text>(eID.second);
			Components::Transform* card_t_m = ecs.getComponent<Components::Transform>(eID.second);

			if(in_c->drag && AEInputCheckCurr(in_c->type))
			{
			}
			else
			{
				cardhand_t->size.x = container_width_ratio * cardhand_t->size_og.x;

				f32 target_x = cardhand_t->pos_onscreen.x + (f32(i) / curr_hand_display.size()) * cardhand_t->size.x - cardhand_t->size.x / 2 + card_t_c->size_og.x/2;
				f32 target_y = cardhand_t->pos_onscreen.y;

				card_t_c->pos.x = target_x;
				card_t_c->pos.y = target_y;
				card_t_c->pos_onscreen = card_t_c->pos;

				card_t_m->pos.x = target_x - card_t_c->size.x * 0.40f;
				card_t_m->pos.y = target_y + card_t_c->size.y * 0.36f;
				card_t_m->pos_onscreen = card_t_m->pos;

				mesh_c->z = this->z;
				text_m->z = this->z + 1;
				i++;
			}

		}
		i = 0;
		
	}

	void CardHand::generateCards()
	{
		EntityComponent::ComponentTypeID cID = EntityComponent::getComponentTypeID<Components::Card_Storage>();

		if (!ecs.getBitMask()[tbsptr->current()].test(cID)) return;

		Components::Card_Storage* cs = ecs.getComponent<Components::Card_Storage>(tbsptr->current());

		for (size_t i = 0; i < cs->data_card_hand.size(); i++)
		
		{
			Entity cardID_ =  cs->data_card_hand.at(i);
			this->curr_card_id.push_back(cardID_);
			this->activate.push_back(false);

			EntityComponent::ComponentTypeID aID = EntityComponent::getComponentTypeID<Components::Name>();
			if (ecs.getBitMask()[i].test(aID)) {};

			Components::Name* name = ecs.getComponent<Components::Name>(cardID_);
			Components::Card_Value* a = ecs.getComponent<Components::Card_Value>(cardID_);
			Components::Card_Cost* c = ecs.getComponent<Components::Card_Cost>(cardID_);
			Components::image_location* cimg = ecs.getComponent<Components::image_location>(cardID_);
			AEGfxTexture* texture = nullptr;

			texture = TF.getTextureFromCardMap(cimg->location);

			Entity eid = ecs.createEntity();

			this->curr_hand_display.push_back(selectableCard_create(eid, ecs, *mfptr, 0, -500, 162, 264, 0, this->z, texture, 
				[this, eid]
				{ 
					if(gbsptr->getPlayerPhase() != PhaseSystem::PlayerPhase::PLAYER_ANIMATION)
					{
						this->gbptr->unselect_card();
						this->activate_card(eid);
					}
				},
			static_cast<s32>(c->value), *this->cdptr, cs->data_card_hand.at(i)));

			
		}
	}

	void CardHand::activate_card(Entity e)
	{
		for (int i = 0; i < this->curr_hand_display.size(); i++)
		{
			if (this->curr_hand_display[i].first == e)
			{
				this->activate[i] = true;
				AF.sfx.play(0);
			}
		}
		AF.sfx.resetAudio();
	}

	void CardHand::remove_card(EntityComponent::Registry& ecs, int index)
	{
		if (this->curr_hand_display.empty() || this->curr_card_id.empty() ||
			this->activate.empty()) return;

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

	Entity CardHand::getID() const
	{
		return this->id;
	}

	void CardHand::card_interaction_free()
	{
		tbsptr = nullptr;
		gbsptr = nullptr;
		mfptr = nullptr;
		gbptr = nullptr;
		tfptr = nullptr;
		cbsptr = nullptr;

		for (std::pair<Entity, Entity> i : curr_hand_display)
		{
			ecs.destroyEntity(i.first);
			ecs.destroyEntity(i.second);
		}
		
		curr_hand_display.clear();
		curr_card_id.clear();
		activate.clear();
		

		ecs.destroyEntity(id);

	}

	void card_onClick(EntityComponent::Registry& ecs, std::pair<Entity, Entity> id)
	{
		Entity first = id.first;
		Entity second = id.second;

		Components::Timer* timer = ecs.getComponent<Components::Timer>(first);

		Components::Transform* t1 = ecs.getComponent<Components::Transform>(first);
		Components::Color* c1 = ecs.getComponent<Components::Color>(first);

		Components::Transform* t2 = ecs.getComponent<Components::Transform>(second);
		Components::Color* c2 = ecs.getComponent<Components::Color>(second);

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		c1->d_color.r = c1->c_color.r - minimum + (1.f - minimum) * lerp;
		c1->d_color.b = c1->c_color.b - minimum + (1.f - minimum) * lerp;
		c1->d_color.g = c1->c_color.g - minimum + (1.f - minimum) * lerp;
		//t1->pos_onscreen.y = t1->pos.y + lerp * minimum * t1->size.y/6;

		c2->d_color.r = c2->c_color.r - minimum + (1.f - minimum) * lerp;
		c2->d_color.b = c2->c_color.b - minimum + (1.f - minimum) * lerp;
		c2->d_color.g = c2->c_color.g - minimum + (1.f - minimum) * lerp;
	}

	void card_offClick(EntityComponent::Registry& ecs, std::pair<Entity, Entity> id)
	{
		Entity first = id.first;
		Entity second = id.second;

		Components::Timer* timer = ecs.getComponent<Components::Timer>(first);

		Components::Transform* t1 = ecs.getComponent<Components::Transform>(first);
		Components::Color* c1 = ecs.getComponent<Components::Color>(first);

		Components::Transform* t2 = ecs.getComponent<Components::Transform>(first);
		Components::Color* c2 = ecs.getComponent<Components::Color>(first);

		c1->d_color = c1->c_color;
		t1->pos_onscreen.y = t1->pos.y;

		c2->d_color = c2->c_color;
		t2->pos_onscreen.y = t2->pos.y;

	}

	void card_onHover(EntityComponent::Registry& ecs, CardInformation::CardDisplay& cd, std::pair<Entity, Entity> id, Entity card_data, s32 z)
	{
		Entity first = id.first;
		Entity second = id.second;

		cd.setStateOn(true);
		cd.setCurrentCard(card_data);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(first);
		Components::Input* i = ecs.getComponent<Components::Input>(first);

		Components::Transform* t1 = ecs.getComponent<Components::Transform>(first);
		Components::Color* c1 = ecs.getComponent<Components::Color>(first);
		Components::Mesh* m1 = ecs.getComponent<Components::Mesh>(first);

		Components::Transform* t2 = ecs.getComponent<Components::Transform>(second);
		Components::Color* c2 = ecs.getComponent<Components::Color>(second);
		Components::Text* m2 = ecs.getComponent<Components::Text>(second);


		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		m1->z = z + 1;
		m2->z = z + 2;
		i->z = z + 1;

		c1->d_color.r = minimum + (1.f - minimum) * lerp;
		c1->d_color.b = minimum + (1.f - minimum) * lerp;
		c1->d_color.g = minimum + (1.f - minimum) * lerp;

		t1->pos_onscreen.y = t1->pos.y + minimum + (1.f - minimum) * lerp * t1->size_og.y / 4;

		t1->size.y = t1->size_og.y * 1.2f;
		t1->size.x = t1->size_og.x * 4.f / 3.f * 1.2f;

		c2->d_color.r = minimum + (1.f - minimum) * lerp;
		c2->d_color.b = minimum + (1.f - minimum) * lerp;
		c2->d_color.g = minimum + (1.f - minimum) * lerp;

		t2->pos_onscreen.y = t2->pos.y + minimum + (1.f - minimum) * lerp * t1->size_og.y / 4;
		t2->size.x = t2->size_og.x * 4.f / 3.f;

	}
	void card_onDrag(EntityComponent::Registry& ecs, CardInformation::CardDisplay& cd, std::pair<Entity, Entity> id, Entity card_data, s32 z)
	{

		Entity first = id.first;
		Entity second = id.second;

		cd.setStateOn(false);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(first);
		Components::Input* i = ecs.getComponent<Components::Input>(first);

		Components::Transform* t1 = ecs.getComponent<Components::Transform>(first);
		Components::Color* c1 = ecs.getComponent<Components::Color>(first);
		Components::Mesh* m1 = ecs.getComponent<Components::Mesh>(first);

		Components::Transform* t2 = ecs.getComponent<Components::Transform>(second);
		Components::Color* c2 = ecs.getComponent<Components::Color>(second);
		Components::Text* m2 = ecs.getComponent<Components::Text>(second);


		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		m1->z = z + 1;
		m2->z = z + 2;
		i->z = 0;

		c1->d_color.r = minimum + (1.f - minimum) * lerp;
		c1->d_color.b = minimum + (1.f - minimum) * lerp;
		c1->d_color.g = minimum + (1.f - minimum) * lerp;
		c1->d_color.a = 0.2f;


		t1->size.y = t1->size_og.y * 1.2f;
		t1->size.x = t1->size_og.x * 4.f / 3.f * 1.2f;

		c2->d_color.r = minimum + (1.f - minimum) * lerp;
		c2->d_color.b = minimum + (1.f - minimum) * lerp;
		c2->d_color.g = minimum + (1.f - minimum) * lerp;
		c2->d_color.a = 0.2f;

		t2->pos_onscreen.y = t2->pos.y + minimum + (1.f - minimum) * lerp * t1->size_og.y / 4;
		t2->size.x = t2->size_og.x * 4.f / 3.f;

		s32 mousex, mousey;

		AEInputGetCursorPosition(&mousex, &mousey);

		mousex = mousex - f32(AEGfxGetWindowWidth()) * 0.5f;
		mousey = -mousey + f32(AEGfxGetWindowHeight()) * 0.5f;

		mousex = AEClamp(mousex, AEGfxGetWinMinX(), AEGfxGetWinMaxX());
		mousey = AEClamp(mousey, AEGfxGetWinMinY(), AEGfxGetWinMaxY());

	
		t1->pos_onscreen.x = f32(mousex);
		t1->pos_onscreen.y = f32(mousey);
		t2->pos_onscreen.x = t1->pos_onscreen.x - t1->size.x * 0.40f;
		t2->pos_onscreen.y = t1->pos_onscreen.y + t1->size.y * 0.36f;

	}
	//void card_offDrag(EntityComponent::Registry& ecs, CardInformation::CardDisplay& cd, std::pair<Entity, Entity> id, Entity card_data)
	//{
	//	Entity first = id.first;
	//	Entity second = id.second;

	//	cd.setStateOn(false);
	//	cd.setCurrentCard(card_data);

	//	Components::Timer* timer = ecs.getComponent<Components::Timer>(first);
	//	Components::Input* i = ecs.getComponent<Components::Input>(first);

	//	Components::Transform* t1 = ecs.getComponent<Components::Transform>(first);
	//	Components::Color* c1 = ecs.getComponent<Components::Color>(first);
	//	Components::Mesh* m1 = ecs.getComponent<Components::Mesh>(first);

	//	Components::Transform* t2 = ecs.getComponent<Components::Transform>(second);
	//	Components::Color* c2 = ecs.getComponent<Components::Color>(second);
	//	Components::Text* m2 = ecs.getComponent<Components::Text>(second);


	//	f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
	//	f32 minimum = 0.6f;

	//	m1->z = 31;
	//	m2->z = 31;
	//	i->z = 31;

	//	c1->d_color.r = minimum + (1.f - minimum) * lerp;
	//	c1->d_color.b = minimum + (1.f - minimum) * lerp;
	//	c1->d_color.g = minimum + (1.f - minimum) * lerp;


	//	t1->size.y = t1->size_og.y * 1.2f;
	//	t1->size.x = t1->size_og.x * 4.f / 3.f * 1.2f;

	//	c2->d_color.r = minimum + (1.f - minimum) * lerp;
	//	c2->d_color.b = minimum + (1.f - minimum) * lerp;
	//	c2->d_color.g = minimum + (1.f - minimum) * lerp;

	//	t2->pos_onscreen.y = t2->pos.y + minimum + (1.f - minimum) * lerp * t1->size_og.y / 4;
	//	t2->size.x = t2->size_og.x * 4.f / 3.f;

	//	s32 mousex, mousey;

	//	AEInputGetCursorPosition(&mousex, &mousey);

	//	mousex = mousex - f32(AEGfxGetWindowWidth()) * 0.5f;
	//	mousey = -mousey + f32(AEGfxGetWindowHeight()) * 0.5f;

	//	mousex = AEClamp(mousex, AEGfxGetWinMinX(), AEGfxGetWinMaxX());
	//	mousey = AEClamp(mousey, AEGfxGetWinMinX(), AEGfxGetWinMaxY());

	//	t1->pos_onscreen.x = f32(mousex);
	//	t1->pos_onscreen.y = f32(mousey);
	//	t2->pos_onscreen.x = f32(mousex);
	//	t2->pos_onscreen.y = f32(mousey);

	//	std::cout << "drag";
	//}



	void card_offHover(EntityComponent::Registry& ecs, CardInformation::CardDisplay& cd, std::pair<Entity, Entity> id, s32 z)
	{

		Entity first = id.first;
		Entity second = id.second;

		Components::Timer* timer = ecs.getComponent<Components::Timer>(first);
		Components::Input* i = ecs.getComponent<Components::Input>(first);

		Components::Transform* t1 = ecs.getComponent<Components::Transform>(first);
		Components::Color* c1 = ecs.getComponent<Components::Color>(first);
		Components::Mesh* m1 = ecs.getComponent<Components::Mesh>(first);

		Components::Transform* t2 = ecs.getComponent<Components::Transform>(second);
		Components::Color* c2 = ecs.getComponent<Components::Color>(second);
		Components::Text* m2 = ecs.getComponent<Components::Text>(second);


		m1->z = z; // card mesh
		m2->z = z + 1; // mana text stays above card
		i->z = 30;

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;
		t1->size.y = t1->size_og.y;
		t1->size.x = t1->size_og.x * 4.f/3.f;

		c1->d_color = c1->c_color;
		c2->d_color = c2->c_color;
	/*	
		t1->pos_onscreen.y = t1->pos.y + minimum + (1.f - minimum) * lerp * t1->size.y / 4;

		t2->pos_onscreen.y = t2->pos.y + minimum + (1.f - minimum) * lerp * t1->size.y / 4;
		*/
		t2->size.y = t2->size_og.y;
		t2->size.x = t2->size_og.x;

		timer->seconds = 0;
	}


	std::pair<Entity, Entity> selectableCard_create(Entity id, EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, AEGfxTexture* pTex, std::function<void()> fp, s32 cost, CardInformation::CardDisplay& cd, Entity card_data)
	{
		//default player values
		Entity mana = selectableCard_mana(ecs, x,y, width, height, cost, z);
		std::pair<Entity, Entity> result{id, mana};

		Components::Transform trans{ {x,y}, {x,y} ,{width, height}, {3 * width / 4, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Texture texture{ pTex };
		Components::Input input(AEVK_LBUTTON, true, fp, 
								[result, &ecs, &cd, card_data, z] { card_onHover(ecs, cd, result, card_data, z); }, 
								[result, &ecs, &cd, z] { card_offHover(ecs, cd, result, z); },
								30, true, true,
								[result, &ecs, &cd, card_data, z] { card_onDrag(ecs, cd, result, card_data, z ); }
								);
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


		return result;
	}

	Entity selectableCard_mana(EntityComponent::Registry& ecs, f32 pos_x, f32 pos_y, f32 width, f32 height, s32 cost, s32 z)
	{
		f32 x = pos_x - width * 0.40f;
		f32 y = pos_y + height* 0.36f;

		/*card_t_m->pos.x = target_x - card_t_c->size.x * 0.40f;
		card_t_m->pos.y = target_y + card_t_c->size.y * 0.36f;*/
		//card_t_m->pos_onscreen = card_t_m->pos;
	
		Entity id = ecs.createEntity();
	
		Components::Transform trans{ {x,y}, {x,y} ,{0.5f, 0.3f} , {0.5f, 0.3f},0.0f };
		Components::Text text{std::to_string(cost), TF.getFontID(), z};
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::TagClass tag{ Components::Tag::UI };	//add input system for grid
		ecs.addComponent(id, trans);
		ecs.addComponent(id, text);
		ecs.addComponent(id, color);
		ecs.addComponent(id, tag);
		return id;
	}

	void selectableCard_delete(EntityComponent::Registry& ecs, std::pair<Entity,Entity> entity)
	{

		ecs.destroyEntity(entity.first);
		ecs.destroyEntity(entity.second);

	}


	void fun()
	{
	}

	void fu(Entity e)
	{
	}
}
