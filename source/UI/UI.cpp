#include "pch.h"
#include "UI.h"


namespace UI
{

	void UIManager::init(Scene& scene, MeshFactory& mf, TextureFactory::TextureFactory& tf)
	{
		//========================================== buttons ======================================
		//end turn button
		Entity end_b = ui_button_texture(ecs, mf, tf.getTextureUI(2), 0.7F * AEGfxGetWinMaxX(), -0.60F * AEGfxGetWinMaxY(), 300, 0.5 * 200, 0, 30,
			// the lambda function
										 [&scene]
										 {
											 if (scene.getGBS().getGBPhase() != PhaseSystem::GBPhase::MAIN_PHASE) return;
											 scene.getTBS().next(ecs);
										 }
		);

		Entity pause_button = ui_button_texture(ecs, mf, tf.getTextureUI(9), 0.9F * AEGfxGetWinMaxX(), 0.85F * AEGfxGetWinMaxY(), 100, 90, 0, 30, nullptr);

		Entity deck_button = ui_button_texture(ecs, mf, tf.getTextureUI(1), -0.85F * AEGfxGetWinMaxX(), -0.85F * AEGfxGetWinMaxY(), 128, 128, 0, 30, nullptr);

		Entity turn_board = ui_blank_texture(ecs, mf, tf.getTextureUI(8), 0.65F * AEGfxGetWinMaxX(), 0.85F * AEGfxGetWinMaxY(), 225, 80, 0, 30);

		Entity turn_text = ui_text(ecs, mf, tf, 0.57F * AEGfxGetWinMaxX(), 0.82F * AEGfxGetWinMaxY(), 0.55f, 80, 0, 31, "Turn 1");

		Entity bin_button = ui_button_texture(ecs, mf, tf.getTextureUI(0), 0.85F * AEGfxGetWinMaxX(), -0.85F * AEGfxGetWinMaxY(), 128, 128, 0, 30, nullptr);

		this->current_ui.push_back(end_b);
		this->current_ui.push_back(pause_button);
		this->current_ui.push_back(deck_button);
		this->current_ui.push_back(turn_board);
		this->current_ui.push_back(turn_text);
		this->current_ui.push_back(bin_button);

		//========================================== health/stamina bar ======================================
		for (Entity e : scene.entities_store())
		{
			Entity health = ui_hp_bar(ecs, mf, -50, 100, 100, 10, 0, 6);
			Entity blank = ui_blank(ecs, mf, -50, 100, 100, 10, 0, 5);
			std::pair<Entity, Entity> hp{ e, health };
			std::pair<Entity, Entity> hp_blank{ e, blank };
			this->actor_children_list.push_back(hp);
			this->actor_children_list.push_back(hp_blank);

			if(e == playerID)
			{
				Entity stamina = ui_stamina_bar(ecs, mf, 40, -20, 70, 8, 90, 6);
				Entity blank_s = ui_blank(ecs, mf, 40, -20, 70, 8, 90, 5);

				std::pair<Entity, Entity> stam{ e, stamina };
				std::pair<Entity, Entity> stam_b{ e, blank_s };
				this->actor_children_list.push_back(stam);
				this->actor_children_list.push_back(stam_b);
			}
		}

		//========================================== mana bar ======================================
		f32 x = 0.55f * AEGfxGetWinMaxX();
		f32 y = -0.85F * AEGfxGetWinMaxY();
		Entity mana_bar = ui_blank_texture(ecs, mf, tf.getTextureUI(4), x, y, 302, 82, 0, 31);
		for(int i = 0; i < 5; i++)
		{
			Components::TurnBasedStats stats{ 0,0,0,0 };
			ecs.addComponent(mana_bar, stats);
			Entity mana_empty = ui_blank_texture(ecs, mf, tf.getTextureUI(3), i * 40 - 40.f, 20.f , 49, 55, 0, 30);
			std::pair<Entity, Entity> mana{ mana_bar, mana_empty };
			this->mana_children_list.push_back(mana);
		}
		this->current_ui.push_back(mana_bar);
	}

	void UIManager::update(Scene& scene)
	{
		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
		EntityComponent::ComponentTypeID meshID = EntityComponent::getComponentTypeID<Components::Mesh>();
		EntityComponent::ComponentTypeID hpID = EntityComponent::getComponentTypeID<Components::HP>();

		if (scene.getGBS().getGBPhase() == PhaseSystem::GBPhase::MAIN_PHASE)
		{

			int i = 0;
			for (; i < this->actor_children_list.size(); i++)
			{
				bool exist = false;
				for (int j = 0; j < scene.getTBS().get_participant().size(); j++)
				{
					Entity participant = scene.getTBS().get_participant()[j];
					if (!ecs.getBitMask()[this->actor_children_list[i].first].test(hpID)) continue;
					else if (participant == this->actor_children_list[i].first) exist = true;
				}
				if (exist == false)
				{
					ecs.destroyEntity(this->actor_children_list[i].second);
					this->actor_children_list[i] = this->actor_children_list[this->actor_children_list.size() - 1];
					this->actor_children_list.pop_back();
				}
			}
		}
		this->health_update(ecs);
		this->stamina_update(ecs);
		this->mana_update(scene);

		for (std::pair<Entity, Entity> p : this->actor_children_list)
		{

			if (!ecs.getBitMask()[p.first].test(transID)) continue;
			if (!ecs.getBitMask()[p.first].test(meshID)) continue;
			if (!ecs.getBitMask()[p.second].test(transID)) continue;
			if (!ecs.getBitMask()[p.second].test(meshID)) continue;

			Components::Transform* parent_t = ecs.getComponent<Components::Transform>(p.first);
			Components::Mesh* parent_m = ecs.getComponent<Components::Mesh>(p.first);
			Components::Transform* child_t = ecs.getComponent<Components::Transform>(p.second);
			Components::Mesh* child_m = ecs.getComponent<Components::Mesh>(p.second);

			child_t->pos_onscreen.x = parent_t->pos_onscreen.x + child_t->pos.x;
			child_t->pos_onscreen.y = parent_t->pos_onscreen.y + child_t->pos.y;
			child_m->z = parent_m->z + 10;

		}

		for (std::pair<Entity, Entity> p : this->mana_children_list)
		{

			if (!ecs.getBitMask()[p.first].test(transID)) continue;
			if (!ecs.getBitMask()[p.second].test(transID)) continue;

			Components::Transform* parent = ecs.getComponent<Components::Transform>(p.first);
			Components::Transform* child = ecs.getComponent<Components::Transform>(p.second);

			child->pos_onscreen.x = parent->pos_onscreen.x + child->pos.x;
			child->pos_onscreen.y = parent->pos_onscreen.y + child->pos.y;

		}

		for(Entity p : this->current_ui)
		{
		}

	}

	/*void UIManager::ui_cardInformation(EntityComponent::Registry& ecs, Entity id)
	{
		if(card_info_exist == false){
			Entity cardInformation = ui_blank_texture(ecs, mf, TF.getTextureUI(9), 0.9F * AEGfxGetWinMaxX(), 0.85F * AEGfxGetWinMaxY(), 379, 458, 0, 30);

			Components::Timer timer{1.f, 0.f};
			ecs.addComponent(cardInformation, timer);

			this->current_ui.push_back(cardInformation);

			card_info_exist = true;

			EntityComponent::ComponentTypeID nameID = EntityComponent::getComponentTypeID<Components::Name>();
			EntityComponent::ComponentTypeID costID = EntityComponent::getComponentTypeID<Components::Card_Cost>();
			EntityComponent::ComponentTypeID damageID = EntityComponent::getComponentTypeID<Components::Card_Value>();
			if(!ecs.getBitMask()[id].test(nameID)) return;
			if(!ecs.getBitMask()[id].test(costID)) return;
			if(!ecs.getBitMask()[id].test(damageID)) return;

			auto name = ecs.getComponent<Components::Name>(id);
			auto cost = ecs.getComponent<Components::Card_Cost>(id);
			auto damage = ecs.getComponent<Components::Card_Value>(id);

			auto parent_t = ecs.getComponent<Components::Transform>(cardInformation);

			Entity cardName = ui_text(ecs, mf, TF, parent_t->pos_onscreen.x, parent_t->pos_onscreen.y, 0.5f, 0.5f, 0, 31, name->value.c_str());
			Entity cardCost = ui_text(ecs, mf, TF, parent_t->pos_onscreen.x, parent_t->pos_onscreen.y - 50.f, 0.5f, 0.5f, 0, 31, std::to_string(cost->value).c_str());
			Entity cardDamage = ui_text(ecs, mf, TF, parent_t->pos_onscreen.x, parent_t->pos_onscreen.y - 100.f, 0.5f, 0.5f, 0, 31, std::to_string(damage->value).c_str());

			std::pair<Entity, Entity> p_n{cardInformation, cardName};
			std::pair<Entity, Entity> p_c{cardInformation, cardCost};
			std::pair<Entity, Entity> p_d{cardInformation, cardDamage};
			this->mana_children_list.push_back(p_n);
			this->mana_children_list.push_back(p_c);
			this->mana_children_list.push_back(p_d);
		}
	}*/

	

	void UIManager::free(EntityComponent::Registry& ecs)
	{
		for(std::pair<Entity, Entity> p : this->actor_children_list)
		{
			ecs.destroyEntity(p.second);
		}
		this->actor_children_list.clear();

		for (std::pair<Entity, Entity> p : this->mana_children_list)
		{
			ecs.destroyEntity(p.second);
		}
		this->mana_children_list.clear();

		for (Entity p : this->current_ui)
		{
			ecs.destroyEntity(p);
		}
		this->current_ui.clear();

	}

	void UIManager::health_update(EntityComponent::Registry& ecs)
	{

		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
		EntityComponent::ComponentTypeID hpID = EntityComponent::getComponentTypeID<Components::HP>();
		//create bitsets
		EntityComponent::ComponentBitMask objMask;
		objMask.set(transID); objMask.set(hpID);

		for (std::pair<Entity, Entity> p : this->actor_children_list)
		{
			if (!ecs.getBitMask()[p.first].test(hpID)) continue;
			if (!ecs.getBitMask()[p.second].test(hpID)) continue;
			Components::HP* hp_parent = ecs.getComponent<Components::HP>(p.first);

			Components::HP* hp_child = ecs.getComponent<Components::HP>(p.second);
			Components::Transform* transform_child = ecs.getComponent<Components::Transform>(p.second);

			hp_child->c_value = hp_parent->c_value / hp_parent->max_value * hp_child->max_value;
			transform_child->size.x = hp_child->c_value / hp_child->max_value * transform_child->size_og.x;
		}

	}


	void UIManager::stamina_update(EntityComponent::Registry& ecs)
	{

		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
		EntityComponent::ComponentTypeID staID = EntityComponent::getComponentTypeID<Components::TurnBasedStats>();
		//create bitsets
		EntityComponent::ComponentBitMask objMask;
		objMask.set(transID); objMask.set(staID);

		for (std::pair<Entity, Entity> p : this->actor_children_list)
		{
			if (!ecs.getBitMask()[p.first].test(staID)) continue;
			if (!ecs.getBitMask()[p.second].test(staID)) continue;
			Components::TurnBasedStats* sta_parent = ecs.getComponent<Components::TurnBasedStats>(p.first);

			Components::TurnBasedStats* sta_child = ecs.getComponent<Components::TurnBasedStats>(p.second);

			Components::Transform* transform_child = ecs.getComponent<Components::Transform>(p.second);

			sta_child->cur_movSpd = sta_parent->cur_movSpd / sta_parent->max_movSpd * sta_child->max_movSpd;
			transform_child->size.x = sta_child->cur_movSpd / sta_child->max_movSpd * transform_child->size_og.x;
		}

	}

	void UIManager::mana_update(Scene& scene)
	{

		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
		EntityComponent::ComponentTypeID staID = EntityComponent::getComponentTypeID<Components::TurnBasedStats>();
		//create bitsets
		EntityComponent::ComponentBitMask objMask;
		objMask.set(transID); objMask.set(staID);

		std::pair<Entity, Entity> p = this->mana_children_list[0];
		Components::TurnBasedStats* sta_parent = ecs.getComponent<Components::TurnBasedStats>(p.first);
		Components::TurnBasedStats* player_stats = ecs.getComponent<Components::TurnBasedStats>(playerID);
		sta_parent->maxPoints = player_stats->maxPoints;
		sta_parent->points = player_stats->points;

		int empty_blocks_for_display = 5;
		int i = 0;

		if(scene.getGBS().getGBPhase() == PhaseSystem::GBPhase::DRAW_PHASE)
		{
			while (this->mana_children_list.size() > empty_blocks_for_display)
			{
				ecs.destroyEntity(this->mana_children_list[(this->mana_children_list.size() - 1)].second);
				this->mana_children_list.pop_back();
			}
		}
		while(this->mana_children_list.size() - empty_blocks_for_display < sta_parent->points)
		{
			Entity mana = ui_blank_texture(ecs, mf, TF.getTextureUI(5), i * 40 - 40.f, 20.f, 49, 55, 0, 31);
			std::pair<Entity, Entity> mana_p{ p.first , mana };
			this->mana_children_list.push_back(mana_p);
			i++;
		}
		while(this->mana_children_list.size() - empty_blocks_for_display > sta_parent->points)
		{
			ecs.destroyEntity(this->mana_children_list[(this->mana_children_list.size() - 1)].second);
			this->mana_children_list.pop_back();
		}
	}



	Entity UIManager::ui_hp_bar(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)

	{

		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
		Components::Color color{ 0.5f, 1.0f, 0.5f ,1.0f };
		Components::HP hp{ 100 };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, hp);

		return id;
	}

	Entity UIManager::ui_stamina_bar(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},{}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
		Components::Color color{ 0.0f, 0.8f, 1.0f ,1.0f };
		Components::TurnBasedStats stats{0,0,0, 100};
		ecs.addComponent(id, stats);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}

	void button_onHover(EntityComponent::Registry& ecs, Entity id)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(id);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.2f;

		c->d_color.r = minimum + (1.f - minimum) * lerp;
		c->d_color.b = minimum + (1.f - minimum) * lerp;
		c->d_color.g = minimum + (1.f - minimum) * lerp;

	}


	void button_offHover(EntityComponent::Registry& ecs, Entity id)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(id);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		c->d_color = c->c_color;
		timer->seconds = 0.5f;

	}


	Entity UIManager::ui_button(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 0.5, 0.5f, 0.5f ,1.0f };
		Components::Input in(AEVK_LBUTTON, true, func, nullptr, nullptr, 10);	//add input system for grid
		Components::TagClass tag{Components::Tag::UI};	//add input system for grid
		Components::Timer timer{ 1.f, 0.f, true, true };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, in);
		ecs.addComponent(id, tag);
		ecs.addComponent(id, timer);


		return id;
	}

	Entity UIManager::ui_button_texture(EntityComponent::Registry& ecs, MeshFactory& mf, AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Texture tex{texture};
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Input in(AEVK_LBUTTON, true, func, [&ecs, id]{ button_onHover(ecs, id);}, [&ecs, id] { button_offHover(ecs, id); }, 10);	//add input system for grid
		Components::TagClass tag{Components::Tag::UI};	//add input system for grid
		Components::Timer timer{1.f, 0.5f, true, true};
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, tex);
		ecs.addComponent(id, color);
		ecs.addComponent(id, in);
		ecs.addComponent(id, tag);
		ecs.addComponent(id, timer);

		return id;
	}

	Entity UIManager::ui_text(EntityComponent::Registry& ecs, MeshFactory& mf, TextureFactory::TextureFactory& tf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, const char* a)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Text text{ a, tf.getFontID(), z };
		Components::Color color{ 0.8f, 1.0f, 1.0f ,1.0f };
		Components::TagClass tag{ Components::Tag::UI };	//add input system for grid
		ecs.addComponent(id, trans);
		ecs.addComponent(id, text);
		ecs.addComponent(id, color);
		ecs.addComponent(id, tag);


		return id;
	}

	Entity UIManager::ui_anchor(EntityComponent::Registry& ecs, f32 x, f32 y, f32 width, f32 height, f32 rotation)
	{
		Entity id = ecs.createEntity();
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::TagClass tag{ Components::Tag::UI };
		ecs.addComponent(id, tag);
		ecs.addComponent(id, trans);


		return id;
	}

	Entity UIManager::ui_blank(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
		Components::Color color{ 0.0f, 0.0f, 0.0f ,1.0f };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}

	Entity UIManager::ui_blank_texture(EntityComponent::Registry& ecs, MeshFactory& mf, AEGfxTexture* texture,f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Texture tex{ texture };
		Components::TagClass tag{ Components::Tag::UI };
		ecs.addComponent(id, tag);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, tex);

		return id;
	}
}