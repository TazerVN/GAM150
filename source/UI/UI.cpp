//=========================================
//	AUTHOR:		PHAM MINH TUAN
// 
//	EMAIL:		minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//=========================================
#include "global.h"
#include "UI.h"
#include "UIObject.h"


namespace UI
{
	UIManager::UIManager() : hp_children_list{}, mana_children_list{}, current_ui{}, menu{}, pause{}, turn{}
	{
	}

	void UIManager::menu_init()
	{
		this->menu.cur = MenuUI::CURRENT_MENU::MAIN;
		this->menu.init();
	}

	void UIManager::combat_init(Scene& scene)
	{
		this->st.free();
		this->st = UIO::ScreenTransition(true, 0.f, 1.f, 2.f);
		this->pause.current_menu = PauseMenu::CURRENT::EMPTY;
		this->pause.setStateOn(false);
		this->pause.init();
		this->pause.setStateCreate(false);
		::pause = false;

		//hand
		s32 w_width = AEGfxGetWindowWidth();
		s32 w_height = AEGfxGetWindowHeight();

		hand = CardInteraction::CardHand(-0.1f * (f32)w_width, -(f32)w_height / 2.f, (f32)w_width / 2.f, 264.f, scene.getTBS(), scene.getBattleGrid()
										 , scene.getGBS(), info,scene.getCombatSystem(),scene.getHighlightSysten());


		//========================================== buttons ======================================
		//end turn button
		Entity end_b = UIO::ui_button_texture(TF.getTextureUI(2), 0.7F * AEGfxGetWinMaxX(), -0.60F * AEGfxGetWinMaxY(), 300.f, 0.5f * 200.f, 0.f, this->z,
			// the lambda function
										
											  [&scene]
											  {
												  if (!::pause){
													  if (scene.getGBS().getGBPhase() != PhaseSystem::GBPhase::MAIN_PHASE) return;
												  scene.getTBS().next();
												  }
											  }
		);

		Entity pause_button = UIO::ui_button_texture(TF.getTextureUI(9), 0.9F * AEGfxGetWinMaxX(), 0.85F * AEGfxGetWinMaxY(), 100, 90, 0, this->z, [this]
													 {
														 if (!::pause)
														 {
															 this->pause.current_menu = PauseMenu::CURRENT::MAIN;
															 this->pause.setStateCreate(true);
															 this->pause.init();
														 }
													 });

		Entity deck_button = UIO::ui_button_texture(TF.getTextureUI(1), -0.85F * AEGfxGetWinMaxX(), -0.85F * AEGfxGetWinMaxY(), 128, 128, 0, this->z, nullptr);
		Entity deck_text = UIO::ui_text(-0.90F * AEGfxGetWinMaxX(), -0.95F * AEGfxGetWinMaxY(), 0.5f, 0.5f, 0, this->z, "0");
		this->draw_pile = std::make_pair(deck_button, deck_text);

		Entity turn_board = UIO::ui_blank_texture(TF.getTextureUI(8), 0.65F * AEGfxGetWinMaxX(), 0.85F * AEGfxGetWinMaxY(), 225, 80, 0, this->z);

		turn = UIO::ui_text(0.55F * AEGfxGetWinMaxX(), 0.82F * AEGfxGetWinMaxY(), 0.55f, 80, 0, this->z + 1, "Round ");

		Entity bin_button = UIO::ui_button_texture(TF.getTextureUI(0), 0.85F * AEGfxGetWinMaxX(), -0.85F * AEGfxGetWinMaxY(), 128, 128, 0, this->z, nullptr);
		Entity bin_text = UIO::ui_text(0.80F * AEGfxGetWinMaxX(), -0.95F * AEGfxGetWinMaxY(), 0.5f, 0.5f, 0, this->z, "0");
		this->discard_pile = std::make_pair(bin_button, bin_text);

		this->current_ui.push_back(end_b);
		this->current_ui.push_back(pause_button);
		this->current_ui.push_back(deck_button);
		this->current_ui.push_back(deck_text);
		this->current_ui.push_back(turn_board);
		this->current_ui.push_back(turn);
		this->current_ui.push_back(bin_button);
		this->current_ui.push_back(bin_text);

		//========================================== health/stamina bar ======================================
		for (Entity e : scene.entities_store())
		{
			Entity health = UIO::ui_hp_bar(-50.f, 100.f, 100.f, 10.f, 0.f, 6);
			Entity blank = UIO::ui_blank_solid_corner(-50.f, 100.f, 100.f, 10.f, 0.f, 6, 0.0f, 0.0f, 0.0f, 1.0f);
			std::pair<Entity, Entity> hp{ e, health };
			std::pair<Entity, Entity> hp_blank{ e, blank };
			this->hp_children_list.push_back(hp_blank);
			this->hp_children_list.push_back(hp);

			if (e == playerID)
			{
				Entity stamina = UIO::ui_stamina_bar(40, -20, 70, 8, 90, 7);
				Entity blank_s = UIO::ui_blank_solid_corner(40, -20, 70, 8, 90, 6, 0.0f, 0.0f, 0.0f, 1.0f);

				std::pair<Entity, Entity> stam{ e, stamina };
				std::pair<Entity, Entity> stam_b{ e, blank_s };
				this->hp_children_list.push_back(stam_b);
				this->hp_children_list.push_back(stam);
			}
		}

		//========================================== mana bar ======================================
		f32 x = 0.55f * AEGfxGetWinMaxX();
		f32 y = -0.85F * AEGfxGetWinMaxY();
		Entity mana_bar = UIO::ui_blank_texture(TF.getTextureUI(4), x, y, 302, 82, 0, this->z + 2);
		for (int i = 0; i < 5; i++)
		{
			Components::TurnBasedStats stats{ 0,0,0,0 };
			ecs.addComponent(mana_bar, stats);
			Entity mana_empty = UIO::ui_blank_texture(TF.getTextureUI(3), i * 40 - 40.f, 20.f, 49, 55, 0, this->z);
			std::pair<Entity, Entity> mana{ mana_bar, mana_empty };
			this->mana_children_list.push_back(mana);
		}
		this->current_ui.push_back(mana_bar);

	}

	void UIManager::update(Scene& scene)
	{
		if (this->pause.isOn()) return;
		if (this->info.isOn() && !this->info.isCreated())
		{
			info = CardInformation::CardDisplay(-0.65F * AEGfxGetWinMaxX(), 0.35F * AEGfxGetWinMaxY(), 379 * 1.3f, 458 * 1.3f, this->z);
		}
		else if (!this->info.isOn() && this->info.isCreated())
		{
			info.free();
		}

		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
		EntityComponent::ComponentTypeID meshID = EntityComponent::getComponentTypeID<Components::Mesh>();
		EntityComponent::ComponentTypeID hordeID = EntityComponent::getComponentTypeID<Components::Horde_Tag>();

		hand.update_logic();
		info.update();
		vicSelect.update();

		if (scene.getGBS().getGBPhase() == PhaseSystem::GBPhase::MAIN_PHASE)
		{

			int i = 0;
			for (; i < this->hp_children_list.size(); i++)
			{
				Entity parent_ui = this->hp_children_list[i].first;

				Entity goon = scene.getTBS().get_participant()[1];

				if (parent_ui == playerID) continue;

				if (ecs.getBitMask()[goon].test(hordeID))
				{
					auto horde = ecs.getComponent<Components::Horde_Tag>(goon);
					auto find = std::find(horde->goons.begin(), horde->goons.end(), parent_ui);
					if (find != horde->goons.end()) continue;
				}


				ecs.destroyEntity(this->hp_children_list[i].second);
				this->hp_children_list[i] = this->hp_children_list[this->hp_children_list.size() - 1];
				this->hp_children_list.pop_back();
			}
		}

		for (std::pair<Entity, Entity> p : this->hp_children_list)
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

		this->player_effect_update();
		this->discardpile_update();
		this->drawpile_update();
		this->health_update();
		this->stamina_update();
		this->pp_update(scene);
		this->turn_update(scene);

		for (std::pair<Entity, Entity> p : this->mana_children_list)
		{

			if (!ecs.getBitMask()[p.first].test(transID)) continue;
			if (!ecs.getBitMask()[p.second].test(transID)) continue;

			Components::Transform* parent = ecs.getComponent<Components::Transform>(p.first);
			Components::Transform* child = ecs.getComponent<Components::Transform>(p.second);

			child->pos_onscreen.x = parent->pos_onscreen.x + child->pos.x;
			child->pos_onscreen.y = parent->pos_onscreen.y + child->pos.y;

		}

		for (std::pair<Entity, Entity> p : this->intention_children_list)
		{
			Entity parent = p.first;
			Entity child = p.second;
			if (!ecs.getBitMask()[parent].test(transID)) continue;
			if (!ecs.getBitMask()[child].test(transID)) continue;

			Components::Transform* parent_trans = ecs.getComponent<Components::Transform>(parent);
			Components::Transform* child_trans = ecs.getComponent<Components::Transform>(child);

			child_trans->pos_onscreen.x = parent_trans->pos_onscreen.x + child_trans->pos.x;
			child_trans->pos_onscreen.y = parent_trans->pos_onscreen.y + child_trans->pos.y;
		}

		this->st.update();
		this->pause.update();

	}

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

	Victory_Select& UIManager::getVictoryMenu()
	{
		return vicSelect;
	}

	void UIManager::free()
	{
		//menu.free();

		hand.card_interaction_free();
		info.free();
		pause.free();
		st.free();
		vicSelect.free();


		for (std::pair<Entity, Entity> p : this->hp_children_list)
		{
			ecs.destroyEntity(p.second);
		}
		this->hp_children_list.clear();

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

		if (player_effect != 0)
		{
			ecs.destroyEntity(player_effect);
			player_effect = 0;
		}
	}

	void UIManager::health_update()
	{

		EntityComponent::ComponentTypeID hpID = EntityComponent::getComponentTypeID<Components::HP>();
		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();

		for (std::pair<Entity, Entity> p : this->hp_children_list)
		{
			if (!ecs.getBitMask()[p.first].test(hpID)) continue;
			if (!ecs.getBitMask()[p.second].test(hpID)) continue;
			if (!ecs.getBitMask()[p.second].test(transID)) continue;

			Components::HP* hp_parent = ecs.getComponent<Components::HP>(p.first);
			Components::HP* hp_child = ecs.getComponent<Components::HP>(p.second);
			Components::Transform* transform_child = ecs.getComponent<Components::Transform>(p.second);
			Components::Mesh* mesh_parent = ecs.getComponent<Components::Mesh>(p.first);
			Components::Mesh* mesh_child = ecs.getComponent<Components::Mesh>(p.second);

			if (!hp_parent || !hp_child || !transform_child) continue;
			if (hp_parent->max_value <= 0.0f || hp_child->max_value <= 0.0f) continue;

			float ratio = hp_parent->c_value / hp_parent->max_value;

			ratio = AEClamp(ratio, 0.0f, 1.f);
			hp_child->c_value = ratio * hp_child->max_value;
			transform_child->size.x = ratio * transform_child->size_og.x;
			mesh_child->z = mesh_parent->z + 11;
		}

	}


	void UIManager::stamina_update()
	{

		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
		EntityComponent::ComponentTypeID staID = EntityComponent::getComponentTypeID<Components::TurnBasedStats>();
		//create bitsets
		EntityComponent::ComponentBitMask objMask;
		objMask.set(transID); objMask.set(staID);

		for (std::pair<Entity, Entity> p : this->hp_children_list)
		{
			if (!ecs.getBitMask()[p.first].test(staID)) continue;
			if (!ecs.getBitMask()[p.second].test(staID)) continue;
			Components::TurnBasedStats* sta_parent = ecs.getComponent<Components::TurnBasedStats>(p.first);
			Components::Mesh* mesh_parent = ecs.getComponent<Components::Mesh>(p.first);


			Components::TurnBasedStats* sta_child = ecs.getComponent<Components::TurnBasedStats>(p.second);
			Components::Transform* transform_child = ecs.getComponent<Components::Transform>(p.second);
			Components::Mesh* mesh_child = ecs.getComponent<Components::Mesh>(p.second);

			if (sta_child == nullptr) continue;
			sta_child->max_movSpd	= sta_parent->max_movSpd;
			sta_child->cur_movSpd	= int((float)sta_parent->cur_movSpd / (float)sta_parent->max_movSpd * (float)sta_child->max_movSpd);
			transform_child->size.x = (float)sta_child->cur_movSpd / (float)sta_child->max_movSpd * (float)transform_child->size_og.x;
			mesh_child->z = mesh_parent->z + 11;
		}

	}

	void UIManager::player_effect_update()
	{
		Components::TurnBasedStats* sta_parent = ecs.getComponent<Components::TurnBasedStats>(playerID);
		Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(playerID);
		Components::Transform* transform = ecs.getComponent<Components::Transform>(playerID);


		//shield implementation
		if (sta_parent->shields > 0)
		{
			if (player_effect == 0)
			{
				player_effect = UIO::ui_blank_texture_world(TF.getTextureOthers(6), transform->pos_onscreen.x, transform->pos_onscreen.y, 128.f * 1.1f, 128.f * 1.1f, 0, mesh->z);
			}
			else
			{
				auto transform_effect = ecs.getComponent<Components::Transform>(player_effect);
				auto mesh_effect = ecs.getComponent<Components::Mesh>(player_effect);
				if (transform_effect != nullptr && mesh_effect != nullptr)
				{
					transform_effect->pos = transform->pos_onscreen;
					transform_effect->pos_onscreen = transform_effect->pos;
					mesh_effect->z = mesh->z;
				}
			}

		}
		else
		{
			if (player_effect != 0)
			{
				ecs.destroyEntity(player_effect);
				player_effect = 0;
			}
		}


	}

	void UIManager::pp_update(Scene& scene)
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

		if (scene.getGBS().getGBPhase() == PhaseSystem::GBPhase::DRAW_PHASE)
		{
			while (this->mana_children_list.size() > empty_blocks_for_display)
			{
				ecs.destroyEntity(this->mana_children_list[(static_cast<int>(this->mana_children_list.size()) - 1)].second);
				this->mana_children_list.pop_back();
			}
		}
		int i = static_cast<int>(this->mana_children_list.size()) - empty_blocks_for_display;

		while (static_cast<int>(this->mana_children_list.size()) - empty_blocks_for_display < sta_parent->points)
		{
			// Wrap position back to 0 when exceeding maxPoints
			int displayPos = i % sta_parent->maxPoints;

			AEGfxTexture* manaTex = (i < sta_parent->maxPoints)
				? TF.getTextureUI(5)   // normal mana
				: TF.getTextureUI(6);  // bonus mana overlaid on top

			Entity mana = UIO::ui_blank_texture(
				manaTex,
				static_cast<f32>(displayPos) * 40.f - 40.f,
				20.f, 49.f, 55.f, 0.f, this->z + 1
			);
			std::pair<Entity, Entity> mana_p{ p.first, mana };
			this->mana_children_list.push_back(mana_p);
			i++;
		}
		while (static_cast<int>(this->mana_children_list.size()) - empty_blocks_for_display > sta_parent->points)
		{
			ecs.destroyEntity(this->mana_children_list[(static_cast<int>(this->mana_children_list.size()) - 1)].second);
			this->mana_children_list.pop_back();
		}
	}

	void UIManager::turn_update(Scene& scene)
	{
		Components::Text* text = ecs.getComponent<Components::Text>(turn);
		std::string temp = std::to_string(scene.getTBS().round());
		std::string buffer{ "Round " + temp };
		text->text = buffer;
	}

	void UIManager::drawpile_update()
	{
		auto dp = ecs.getComponent<Components::Card_Storage>(playerID);
		auto text = ecs.getComponent<Components::Text>(this->draw_pile.second);

		text->text = std::to_string(dp->data_draw_pile.size()).c_str();
	}
	void UIManager::discardpile_update()
	{
		auto dc = ecs.getComponent<Components::Card_Storage>(playerID);
		auto text = ecs.getComponent<Components::Text>(this->discard_pile.second);

		text->text = std::to_string(dc->data_discard_pile.size()).c_str();
	}
}