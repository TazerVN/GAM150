#include "pch.h"
#include "UI.h"
#include "UIObject.h"


namespace UI
{
	UIManager::UIManager() : actor_children_list{}, mana_children_list{}, current_ui{}
	{
	}

	void UIManager::init(Scene& scene, MeshFactory& mf, TextureFactory::TextureFactory& tf)
	{
		//hand
		s32 w_width = AEGfxGetWindowWidth();
		s32 w_height = AEGfxGetWindowHeight();

		hand = CardInteraction::CardHand(ecs, mf, TF, -0.1f * w_width, -w_height / 2, w_width / 2, 264, scene.getTBS(), scene.getBattleGrid()
										 , scene.getGBS(), info);


		//========================================== buttons ======================================
		//end turn button
		Entity end_b = UIO::ui_button_texture(ecs, mf, tf.getTextureUI(2), 0.7F * AEGfxGetWinMaxX(), -0.60F * AEGfxGetWinMaxY(), 300, 0.5 * 200, 0, 30,
			// the lambda function
										 [&scene]
										 {
											 if (scene.getGBS().getGBPhase() != PhaseSystem::GBPhase::MAIN_PHASE) return;
											 scene.getTBS().next(ecs);
										 }
		);

		Entity pause_button = UIO::ui_button_texture(ecs, mf, tf.getTextureUI(9), 0.9F * AEGfxGetWinMaxX(), 0.85F * AEGfxGetWinMaxY(), 100, 90, 0, 30, [this]{ this->pause.setStateOn(true);});

		Entity deck_button = UIO::ui_button_texture(ecs, mf, tf.getTextureUI(1), -0.85F * AEGfxGetWinMaxX(), -0.85F * AEGfxGetWinMaxY(), 128, 128, 0, 30, nullptr);

		Entity turn_board = UIO::ui_blank_texture(ecs, mf, tf.getTextureUI(8), 0.65F * AEGfxGetWinMaxX(), 0.85F * AEGfxGetWinMaxY(), 225, 80, 0, 30);

		Entity turn_text = UIO::ui_text(ecs, mf, tf, 0.57F * AEGfxGetWinMaxX(), 0.82F * AEGfxGetWinMaxY(), 0.55f, 80, 0, 31, "Turn 1");

		Entity bin_button = UIO::ui_button_texture(ecs, mf, tf.getTextureUI(0), 0.85F * AEGfxGetWinMaxX(), -0.85F * AEGfxGetWinMaxY(), 128, 128, 0, 30, nullptr);

		this->current_ui.push_back(end_b);
		this->current_ui.push_back(pause_button);
		this->current_ui.push_back(deck_button);
		this->current_ui.push_back(turn_board);
		this->current_ui.push_back(turn_text);
		this->current_ui.push_back(bin_button);

		//========================================== health/stamina bar ======================================
		for (Entity e : scene.entities_store())
		{
			Entity health = UIO::ui_hp_bar(ecs, mf, -50, 100, 100, 10, 0, 6);
			Entity blank = UIO::ui_blank(ecs, mf, -50, 100, 100, 10, 0, 6, 0.0f, 0.0f, 0.0f, 1.0f);
			std::pair<Entity, Entity> hp{ e, health };
			std::pair<Entity, Entity> hp_blank{ e, blank };
			this->actor_children_list.push_back(hp_blank);
			this->actor_children_list.push_back(hp);

			if (e == playerID)
			{
				Entity stamina = UIO::ui_stamina_bar(ecs, mf, 40, -20, 70, 8, 90, 6);
				Entity blank_s = UIO::ui_blank(ecs, mf, 40, -20, 70, 8, 90, 6, 0.0f, 0.0f, 0.0f, 1.0f);

				std::pair<Entity, Entity> stam{ e, stamina };
				std::pair<Entity, Entity> stam_b{ e, blank_s };
				this->actor_children_list.push_back(stam_b);
				this->actor_children_list.push_back(stam);
			}
		}

		//========================================== mana bar ======================================
		f32 x = 0.55f * AEGfxGetWinMaxX();
		f32 y = -0.85F * AEGfxGetWinMaxY();
		Entity mana_bar = UIO::ui_blank_texture(ecs, mf, tf.getTextureUI(4), x, y, 302, 82, 0, 32);
		for (int i = 0; i < 5; i++)
		{
			Components::TurnBasedStats stats{ 0,0,0,0 };
			ecs.addComponent(mana_bar, stats);
			Entity mana_empty = UIO::ui_blank_texture(ecs, mf, tf.getTextureUI(3), i * 40 - 40.f, 20.f, 49, 55, 0, 30);
			std::pair<Entity, Entity> mana{ mana_bar, mana_empty };
			this->mana_children_list.push_back(mana);
		}
		this->current_ui.push_back(mana_bar);
	}

	void UIManager::update(Scene& scene, f32 dt)
	{

		if(this->info.isOn() && !this->info.isCreated())
		{
			info = CardInformation::CardDisplay(ecs, mf, -0.75F * AEGfxGetWinMaxX(), 0.50F * AEGfxGetWinMaxY(), 379, 458, 30);
		}
		else if(!this->info.isOn() && this->info.isCreated())
		{
			info.free(ecs);
		}

		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
		EntityComponent::ComponentTypeID meshID = EntityComponent::getComponentTypeID<Components::Mesh>();
		EntityComponent::ComponentTypeID hpID = EntityComponent::getComponentTypeID<Components::HP>();
		EntityComponent::ComponentTypeID hordeID = EntityComponent::getComponentTypeID<Components::Horde_Tag>();

		hand.update_logic(ecs, scene.getTBS(), mf, TF, dt);
		info.update(ecs);

		if (scene.getGBS().getGBPhase() == PhaseSystem::GBPhase::MAIN_PHASE)
		{

			int i = 0;
			for (; i < this->actor_children_list.size(); i++)
			{
				Entity parent_ui = this->actor_children_list[i].first;
				bool exist = false;

				Entity goon = scene.getTBS().get_participant()[1];
				Entity player = playerID;

				if (parent_ui == playerID) continue;

				if (ecs.getBitMask()[goon].test(hordeID))
				{
					auto horde = ecs.getComponent<Components::Horde_Tag>(goon);
					auto find = std::find(horde->goons.begin(), horde->goons.end(), parent_ui);
					if (find != horde->goons.end()) continue;
				}


				ecs.destroyEntity(this->actor_children_list[i].second);
				this->actor_children_list[i] = this->actor_children_list[this->actor_children_list.size() - 1];
				this->actor_children_list.pop_back();
			}
		}

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

		this->health_update(ecs);
		this->stamina_update(ecs);
		this->mana_update(scene);

		for (std::pair<Entity, Entity> p : this->mana_children_list)
		{

			if (!ecs.getBitMask()[p.first].test(transID)) continue;
			if (!ecs.getBitMask()[p.second].test(transID)) continue;

			Components::Transform* parent = ecs.getComponent<Components::Transform>(p.first);
			Components::Transform* child = ecs.getComponent<Components::Transform>(p.second);

			child->pos_onscreen.x = parent->pos_onscreen.x + child->pos.x;
			child->pos_onscreen.y = parent->pos_onscreen.y + child->pos.y;

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

	CardInteraction::CardHand& UIManager::getCardHand()
	{
		return this->hand;
	}
	CardInformation::CardDisplay& UIManager::getCardDisplay()
	{
		return this->info;
	}
	PauseMenu& UIManager::getPauseMenu()
	{
		return pause;
	}



	void UIManager::free(EntityComponent::Registry& ecs)
	{
		hand.card_interaction_free();
		info.free(ecs);
		pause.free(ecs);

		for (std::pair<Entity, Entity> p : this->actor_children_list)
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

		EntityComponent::ComponentTypeID hpID = EntityComponent::getComponentTypeID<Components::HP>();
		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();

		for (std::pair<Entity, Entity> p : this->actor_children_list)
		{
			if (!ecs.getBitMask()[p.first].test(hpID)) continue;
			if (!ecs.getBitMask()[p.second].test(hpID)) continue;
			if (!ecs.getBitMask()[p.second].test(transID)) continue;

			Components::HP* hp_parent = ecs.getComponent<Components::HP>(p.first);

			Components::HP* hp_child = ecs.getComponent<Components::HP>(p.second);
			Components::Transform* transform_child = ecs.getComponent<Components::Transform>(p.second);
			Components::Mesh* mesh_parent = ecs.getComponent<Components::Mesh>(p.first);
			Components::Mesh* mesh_child = ecs.getComponent<Components::Mesh>(p.second);

			//if (!hp_parent || !hp_child || !transform_child) continue;
			//if (hp_parent->max_value <= 0.0f || hp_child->max_value <= 0.0f) continue;

			float ratio = hp_parent->c_value / hp_parent->max_value;

			ratio = AEClamp(ratio, 0.0f, 1.f);
			hp_child->c_value = ratio * hp_child->max_value;
			transform_child->size.x = ratio * transform_child->size_og.x;
			mesh_child->z = mesh_parent->z + 11;
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
			Components::Mesh* mesh_parent = ecs.getComponent<Components::Mesh>(p.first);


			Components::TurnBasedStats* sta_child = ecs.getComponent<Components::TurnBasedStats>(p.second);
			Components::Transform* transform_child = ecs.getComponent<Components::Transform>(p.second);
			Components::Mesh* mesh_child = ecs.getComponent<Components::Mesh>(p.second);

			sta_child->cur_movSpd = sta_parent->cur_movSpd / sta_parent->max_movSpd * sta_child->max_movSpd;
			transform_child->size.x = sta_child->cur_movSpd / sta_child->max_movSpd * transform_child->size_og.x;
			mesh_child->z = mesh_parent->z + 11;
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

		if (scene.getGBS().getGBPhase() == PhaseSystem::GBPhase::DRAW_PHASE)
		{
			while (this->mana_children_list.size() > empty_blocks_for_display)
			{
				ecs.destroyEntity(this->mana_children_list[(this->mana_children_list.size() - 1)].second);
				this->mana_children_list.pop_back();
			}
		}
		while (this->mana_children_list.size() - empty_blocks_for_display < sta_parent->points)
		{
			Entity mana = UIO::ui_blank_texture(ecs, mf, TF.getTextureUI(5), i * 40 - 40.f, 20.f, 49, 55, 0, 31);
			std::pair<Entity, Entity> mana_p{ p.first , mana };
			this->mana_children_list.push_back(mana_p);
			i++;
		}
		while (this->mana_children_list.size() - empty_blocks_for_display > sta_parent->points)
		{
			ecs.destroyEntity(this->mana_children_list[(this->mana_children_list.size() - 1)].second);
			this->mana_children_list.pop_back();
		}
	}


}