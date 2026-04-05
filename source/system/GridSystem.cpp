//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Pham Minh Tuan
//				Zejin Kendreich Dayap Chen
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				minhtuan.pham@digipen.edu
//				chen.z@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================

#include "GridSystem.h"
#include <utility> // for std::move
#include "../util/util.h"
#include "../system/TurnBasedSystem.h"
#include "../system/PhaseSystem.h"
#include "../util/Pathfinding.h"
#include "../global.h"
#include "../ECS/Components.h"
#include "../system/HightlightSystem.h"

float offset = 1.0f;

namespace Grid
{
	void GameBoard::updateCell(s32 x, s32 y)
	{
		//if (is_active)
		//if the current participant ha	s selected card 
				//and selected on the empty cell then return
		prev_x = cur_x;
		prev_y = cur_y;
		cur_x = x;
		cur_y = y;

		if (!(gbsptr->getGBPhase() == PhaseSystem::GBPhase::MAIN_PHASE)) return;
		if (tbsptr->current() != playerID) return;

		switch (gbsptr->getPlayerPhase())
		{
			case PhaseSystem::PlayerPhase::AOE_GRID_SELECT:
			{
				if (tbsptr->is_current_selected_card())
				{
					if (cbsptr->check_within_range(x, y))
					{
						if (pos[x][y] == tbsptr->current())
						{
							unselect_card();
							move_select(x, y);
							return;
						}
					}
					else
					{
						std::cout << "Selected cell is outside range" << '\n';
						PUT << 0 << "Out of Range";
						unselect_card();
						return;
					}
					trigger_play_card(x, y);
				}
				break;
			}
			case PhaseSystem::PlayerPhase::GRID_SELECT:
			{
				Entity cardID = cbsptr->draw_card(playerID, tbsptr->get_selected_cardhand_index());
				Targetting targettingType = ecs.getComponent<Components::Targetting_Component>(cardID)->targetting_type;

				int serialID = ecs.getComponent<Components::Card_ID>(cardID)->value;
				int category = serialID / 1000;
				bool isEventCard = (category == 4);

				std::cout << "Selected card :" << ecs.getComponent<Components::Name>(cardID)->value << '\n';

				if (!cbsptr->check_within_range(x, y))
				{
					std::cout << "Target is outside range" << '\n';
					PUT << 0 << "Out of range!";
					unselect_card();
					return;
				}

				if (isEventCard)
				{
					// Mana Wall is placed on empty cells, not entities
					if (pos[x][y] != -1)
					{
						std::cout << "Mana Wall requires an empty cell" << '\n';
						PUT << 0 << "Select an empty cell";
						unselect_card();
						return;
					}

					trigger_play_card(x, y);
					return;
				}

				// your old behaviour steven - zejin
				if (pos[x][y] != -1)
				{
					if (targettingType != Targetting::SELF && pos[x][y] == tbsptr->current())
					{
						std::cout << "Selected Player initializing Movement" << '\n';
						move_select(x, y);
						return;
					}

					trigger_play_card(x, y);
				}
				else
				{
					std::cout << "Select a valid cell with entity" << '\n';
					unselect_card();
				}
				break;
			}
			case PhaseSystem::PlayerPhase::PLAYER_EXPLORE:
			{
				//check if an empty cell is clicked
				if (selected_part)
				{
					move_trigger(x,y);
				}
				//check if the grid cell with entity is clicked
				else if (!selected_part && this->pos[x][y] != -1)
				{
					move_select(x, y);
				}
				break;
			}
			default:
				break;
		}
	}

	void GameBoard::trigger_play_card(s32 x, s32 y)
	{
		cbsptr->set_targetted_ent(pos[x][y]);
		cbsptr->set_targetted_xy(x, y);
		gbsptr->set_GBPhase(PhaseSystem::GBPhase::PLAYER_RESOLUTION);
		gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_ANIMATION);
		/*gbsptr->GBPTriggered()[static_cast<size_t>(PhaseSystem::GBPhase::PLAYER_RESOLUTION)] = true;*/

		Components::Card_Storage* cs = ecs.getComponent<Components::Card_Storage>(playerID);
		Components::Animation_Actor* anim = ecs.getComponent<Components::Animation_Actor>(playerID);
		Entity cur_card = cs->data_card_hand[tbsptr->get_selected_cardhand_index()];

		Components::Targetting_Component* tc = ecs.getComponent<Components::Targetting_Component>(cur_card);
		if(tc->targetting_type == Targetting::SINGLE_TARGET && tc->range <= 2.f){
			anim->setType(Components::AnimationType::ATTACK_MELEE);
		}
		else
		{
			anim->setType(Components::AnimationType::ATTACK_RANGE);
		}
	}

	void GameBoard::unselect_card()
	{
		tbsptr->set_selected_card(false);
		gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_EXPLORE);
		hlptr->unhighlight_atk_cells();
		placementDirection = 0;
		
	}

	void GameBoard::move_trigger(s32 const& x, s32 const& y)
	{
		//check if it is within movement range 

		if (!cbsptr->check_within_range(x, y))
		{
			std::cout << "Outside movement range" << '\n';
			PUT << 0 << "Outside range";
			unselect_movement();
			return;
		}

		if (this->pos[x][y] != -1)
		{
			std::cout << "Cannot move onto another entity" << '\n';
			PUT << 0 << "Already Occupied";
			unselect_movement();
			return;
		}
		walkable[cur_y * MAX_I + cur_x] = 1;
		//move the entity
		this->moveEntity(this->cur, x, y);
		unselect_movement();

		//tbsptr->show_stats(ecs);
		gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_ANIMATION);
	}

	void GameBoard::move_select(s32 const& x, s32 const& y)
	{
		if(tbsptr->is_current_selected_card())this->unselect_card();
		if (pos[x][y] != tbsptr->current())
		{
			std::cout << "Cannot select this entity! " << pos[x][y] << '\n';
			PUT << 0 << "Cannot select this Entity";
			return;
		}

		if(ecs.getComponent<Components::TurnBasedStats>(playerID)->cur_movSpd == 0)
		{
			PUT << 0 << "Out of stamina";
			return;
		}


		this->cur = this->pos[x][y];
		selected_part = true;
		evsptr->template_pool[HIGHLIGHT_EVENT].triggered = true;
		evsptr->template_pool[HIGHLIGHT_EVENT].returned_value = highlight_tag::MOVE_HIGHLIGHT;
	
	}
	void cell_onHover(Entity cell_id, Entity character)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(cell_id);
		Components::Timer* t = ecs.getComponent<Components::Timer>(cell_id);

		f32 lerp = t->seconds / (t->max_seconds / 2.f) >= 1.f ? t->max_seconds - t->seconds : t->seconds;
		f32 minimum = 0.6f;

		c->d_color.r = minimum + (1.f - minimum) * lerp;
		c->d_color.b = minimum + (1.f - minimum) * lerp;
		c->d_color.g = minimum + (1.f - minimum) * lerp;


		if (character != -1)
		{
			Components::Color* cc = ecs.getComponent<Components::Color>(character);
			
			if (cc != nullptr)
			{
				cc->d_color.r = minimum + (1.f - minimum) * lerp;
				cc->d_color.b = minimum + (1.f - minimum) * lerp;
				cc->d_color.g = minimum + (1.f - minimum) * lerp;
			}
		}
	}

	void cell_offHover( Entity cell_id, Entity character)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(cell_id);
		Components::Timer* t = ecs.getComponent<Components::Timer>(cell_id);

		c->d_color.r = c->c_color.r;
		c->d_color.g = c->c_color.g;
		c->d_color.b = c->c_color.b;
		t->seconds = 0.5f;

		if (character != -1)
		{
			Components::Color* cc = ecs.getComponent<Components::Color>(character);
			if (cc != nullptr)
			{
				cc->d_color.r = cc->c_color.r;
				cc->d_color.g = cc->c_color.g;
				cc->d_color.b = cc->c_color.b;
			}
		}
	}

	Entity GameBoard::create_cells(AEVec2 _pos, AEVec2 size, f32 rotation, AEGfxTexture* pTex, s32 x, s32 y, s32 z)
	{
		Entity id = ecs.createEntity();

		Components::Transform trans{ _pos, _pos, size, {size.x / 2, size.y / 2},rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Texture texture{ pTex };
		Components::Input in
		(
			AEVK_LBUTTON, true,
			[x, y, this]
			{
				this->updateCell(x, y);
			},
			[x, y, id, this]
			{
				cell_onHover(id, this->pos[x][y]);

				Entity hovered = this->pos[x][y];
				hlptr->highlight_enemy_cells(hovered);

				if (gbsptr->getGBPhase() == PhaseSystem::GBPhase::MAIN_PHASE)
				{
					for (Components::GridCell a :hlptr->aoe_highlighted_cells)
					{
						hlptr->aoe_highlight_activate[int(a.x)][int(a.y)] = 0;
					}
					hlptr->aoe_highlighted_cells.clear();

					for (Components::GridCell& a : hlptr->move_trail_highlighted_cells)
					{
						hlptr->move_trail_highlight_activate[int(a.x)][int(a.y)] = 0;
					}
					hlptr->move_trail_highlighted_cells.clear();
				}

				if (this->selected_part)
				{
					this->draw_movement_trail(x, y);
				}

				if (gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::AOE_GRID_SELECT)
				{
					this->func_aoe_hightlight_cells(x, y);
				}
			},
			[x, y, id, this]
			{
				//hlptr->unhighlight_enemy_cells();
				cell_offHover(id, this->pos[x][y]);
			},1
		);	//add input system for grid
		Components::GridCell gc{ x,y };
		Components::Timer timer{ 1.f , 0.5f, true, true };

		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);
		ecs.addComponent(id, gc);
		ecs.addComponent(id, in);
		ecs.addComponent(id, timer);

		cells_entity_id.push_back(id);

		return id;
	}
	void GameBoard::init(TBS::TurnBasedSystem* tbsys, EventPool<highlight_tag>& evs, PhaseSystem::GameBoardState& gb,
		CombatNameSpace::CombatSystem& cbs,HighlightSystem& hl, AEGfxTexture* pTex, f32 ox, f32 oy)
	{
		tbsptr = tbsys;
		evsptr = &evs;
		gbsptr = &gb;
		cbsptr = &cbs;
		hlptr = &hl;

		this->offset.x = ox;
		this->offset.y = oy;
		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				f32 x = this->offset.x + (i - j) * CELL_WIDTH / 2; /*+ offset*//*if offset is required*/
				f32 y = this->offset.y - (i + j) * CELL_HEIGHT / 4; /*+ offset*//*if offset is required*/

				cells[i][j] = create_cells({ x,y }, { 128.f,128.f }, 0.f, pTex, i, j, 0);
				this->pos[i][j] = Entity(-1);
				//this->activate[i][j] = false;
				walkable[j * MAX_I + i] = 1;
			}
		}
	}

	void GameBoard::placeEntity(Entity e, s32 x, s32 y)
	{
		if (x >= MAX_I || x < 0 || y >= MAX_J || y < 0) return;

		this->pos[x][y] = e;
		this->walkable[y * MAX_I + x] = 0;

		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
		EntityComponent::ComponentTypeID gdID = EntityComponent::getComponentTypeID<Components::gridData>();
		if (!ecs.getBitMask()[e].test(gdID)) return;

		if (!ecs.getBitMask()[e].test(transID)) return;

		Components::Transform* transform = ecs.getComponent<Components::Transform>(e);
		Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(e);
		Components::gridData* gd = ecs.getComponent<Components::gridData>(e);
		//Components::Input* input = ecs.getComponent<Components::Input>(e);

		gd->prev_x = x;
		gd->prev_y = y;
		gd->x = x;
		gd->y = y;

		transform->pos.x = this->offset.x + (x - y) * CELL_WIDTH / 2;
		transform->pos.y = transform->size.y / 3 + this->offset.y - (x + y) * CELL_HEIGHT / 4;
		transform->pos_onscreen = transform->pos;
		mesh->z = 2 + y * MAX_J + x;


		Entity current_cell = this->cells[x][y];
		EntityComponent::ComponentTypeID colorID = EntityComponent::getComponentTypeID<Components::Color>();
		if (!ecs.getBitMask()[current_cell].test(colorID)) return;

		Components::Color* color = ecs.getComponent<Components::Color>(current_cell);
		color->d_color.g = 0.5f;
		color->d_color.r = 0.5f;


		auto actor_in = ecs.getComponent<Components::Input>(e);
		auto actor_pos = ecs.getComponent<Components::gridData>(e);

		if(actor_in != nullptr && actor_pos != nullptr){

			auto cell = ecs.getComponent<Components::Input>(this->cells[actor_pos->x][actor_pos->y]);
			actor_in->onClick = [this, e]()
				{

					auto actor_pos = ecs.getComponent<Components::gridData>(e); // fresh fetch
					//auto actor_in = ecs.getComponent<Components::Input>(e);

					//if (actor_pos == nullptr) return;
					this->updateCell(actor_pos->x, actor_pos->y);
				};
			actor_in->onHover = [this, e]()
				{
					auto actor_pos = ecs.getComponent<Components::gridData>(e); // fresh fetch
					auto cell = ecs.getComponent<Components::Input>(this->cells[actor_pos->x][actor_pos->y]);
					//if (actor_pos == nullptr) return;
					if (cell != nullptr) cell->allow_hover = false;
					cell_onHover(this->cells[actor_pos->x][actor_pos->y] ,e);
				};
			actor_in->offHover = [this, e]()
				{
					auto actor_pos = ecs.getComponent<Components::gridData>(e); // fresh fetch
					auto cell = ecs.getComponent<Components::Input>(this->cells[actor_pos->x][actor_pos->y]);
					if(cell != nullptr) cell->allow_hover = true;
					//if (actor_pos == nullptr) return;
					cell_offHover(this->cells[actor_pos->x][actor_pos->y], e);
				};
				actor_in->z = cell->z + 1;


		}

	}

	void GameBoard::moveEntity(Entity e, s32 x, s32 y)
	{
		Components::gridData* gd = ecs.getComponent<Components::gridData>(e);
		
		int start_i{ gd->x }, start_j{ gd->y };
		int dist = grid_dist_manhattan(start_i, start_j, x, y);

		EntityComponent::ComponentTypeID astarID = EntityComponent::getComponentTypeID<Components::AStarResult>();
		EntityComponent::ComponentTypeID animID = EntityComponent::getComponentTypeID<Components::Animation_Actor>();

		if (ecs.getBitMask()[e].test(astarID) && ecs.getBitMask()[e].test(animID))
		{

			Components::GridCell s{ start_i, start_j };
			Components::GridCell g{ x,  y };

			Components::AStarResult* astar = ecs.getComponent<Components::AStarResult>(e);
			astar->path = AStar_FindPath_Grid4(MAX_I, MAX_J, walkable, s, g).path;

			if(!astar->path.empty())
			{ 
				Components::Animation_Actor* anim = ecs.getComponent<Components::Animation_Actor>(e);
				anim->setType(Components::AnimationType::MOVING);

				this->pos[start_i][start_j] = Entity(-1);
				this->walkable[start_j * MAX_I + start_i] = 1;

				this->pos[x][y] = e;
				this->walkable[y * MAX_I + x] = 0;

				if (!gameData.win)
				{
					ecs.getComponent<Components::TurnBasedStats>(e)->cur_movSpd -= dist;
				}
			}
			else
			{
				std::cout << "Cant find path!!" << '\n';
			}
		}
		// update grid data
		gd->prev_x = gd->x; gd->prev_y = gd->y;
		gd->x = x; gd->y = y;
	}

	void GameBoard::update()
	{
		//check if the mouse is on board
		mouse_on_board = false;

		if (getGridCellFromMouse(this->hx, this->hy))
			mouse_on_board = true;

		if (!mouse_on_board)
		{
			//hlptr->unhighlight_cells();
		}

		bool isGust = false;
		bool isManaWall = false;

		if (tbsptr->is_current_selected_card())
		{
			int idx = tbsptr->get_selected_cardhand_index();
			if (idx >= 0)
			{
				Entity cardID = cbsptr->draw_card(playerID, idx);
				if (cardID != Components::NULL_INDEX)
				{
					Components::Card_ID* idComp = ecs.getComponent<Components::Card_ID>(cardID);
					if (idComp)
					{
						int serialID = idComp->value;
						isManaWall = (serialID == 4220);
						isGust = (serialID == 4120);
					}
				}
			}
		}

		if (tbsptr->is_current_selected_card()
			&& AEInputCheckTriggered(AEVK_RBUTTON))
		{
			unselect_card();
		}
		if (selected_part && AEInputCheckTriggered(AEVK_RBUTTON))
		{
			unselect_movement();
		}
		if (tbsptr->is_current_selected_card()
			&& AEInputCheckTriggered(AEVK_R))
		{
			bool canRotate = isManaWall || isGust;

			if (gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::AOE_GRID_SELECT && canRotate)
			{
				placementDirection = (placementDirection + 1) % 4;

				for (Components::GridCell& a : hlptr->aoe_highlighted_cells)
				{
					hlptr->aoe_highlight_activate[int(a.x)][int(a.y)] = 0;
				}
				hlptr->aoe_highlighted_cells.clear();

				std::cout << "Direction = " << placementDirection << '\n';
			}
		}

		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				//update all cells
				EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
				EntityComponent::ComponentTypeID colorID = EntityComponent::getComponentTypeID<Components::Color>();

				Components::Transform* transform = ecs.getComponent<Components::Transform>(this->cells[i][j]);
				Components::Color* color = ecs.getComponent<Components::Color>(this->cells[i][j]);


				if (hlptr->enemy_mov_highlight_activate[i][j])
				{
					color->d_color.r = 1.f;
					color->d_color.g = 1.f;
					color->d_color.b = 0.f;
				}
				if (hlptr->enemy_atk_highlight_activate[i][j])
				{
					if (hlptr->enemy_mov_highlight_activate[i][j])
					{
						color->d_color.g -= 0.5f;
					}
					else
					{
						color->d_color.r = 1.f;
						color->d_color.g = 0.f;
						color->d_color.b = 0.f;
					}
				}

				//uncommment this and comment the below code if you want normal messy ahh color
				//color->d_color.r = color->d_color.r + hlptr->highlight_activate[i][j].r;
				//color->d_color.g = color->d_color.g + hlptr->highlight_activate[i][j].g;
				//color->d_color.b = color->d_color.b + hlptr->highlight_activate[i][j].b;

				//uncomment this if you dont like dim but dont forget to uncomment the above or the CODE WILL BERAK
				//base to 0.5f if you want to dim
				if (selected_part || tbsptr->is_current_selected_card())
				{
					color->d_color.r = 0.5f + hlptr->highlight_activate[i][j].r;
					color->d_color.g = 0.5f + hlptr->highlight_activate[i][j].g;
					color->d_color.b = 0.5f + hlptr->highlight_activate[i][j].b;
				}

				if (hlptr->aoe_highlight_activate[i][j])
				{
					if (isGust)
					{
						int gustLevel = hlptr->aoe_highlight_activate[i][j];

						if (gustLevel == 1) // shaft
						{
							color->d_color.r = 0.35f;
							color->d_color.g = 0.75f;
							color->d_color.b = 0.95f;
						}
						else if (gustLevel == 2) // arrow head
						{
							color->d_color.r = 0.70f;
							color->d_color.g = 0.90f;
							color->d_color.b = 1.00f;
						}
						else if (gustLevel >= 3) // tip
						{
							color->d_color.r = 1.00f;
							color->d_color.g = 1.00f;
							color->d_color.b = 1.00f;
						}
					}
					else
					{
						color->d_color.r -= (hlptr->aoe_highlight_activate[i][j] == 2) ? 0.6f : 0.4f;
						color->d_color.g -= 0.8f;
						color->d_color.b -= 0.8f;
					}
				}

				if (hlptr->move_trail_highlight_activate[i][j])
				{
					color->d_color.r -= 0.99f;
					color->d_color.g -= 0.99f;
					color->d_color.b -= 0.4f;
				}

				

				//======================update entity cell=====================================
				if (this->pos[i][j] != -1)
				{
					Entity e = this->pos[i][j];

					if (!ecs.getBitMask()[e].test(transID)) return;
					if (!ecs.getBitMask()[e].test(colorID)) return;

					transform = ecs.getComponent<Components::Transform>(e);
					color = ecs.getComponent<Components::Color>(e);

					Entity current_cell = this->cells[i][j];
					colorID = EntityComponent::getComponentTypeID<Components::Color>();
					if (!ecs.getBitMask()[current_cell].test(colorID)) return;

					color = ecs.getComponent<Components::Color>(current_cell);
					color->d_color.g = color->d_color.g - 0.4f;
					color->d_color.r = color->d_color.r - 0.4f;
				}

				if (this->in_walkable_debug && this->walkable[j * MAX_I + i])
				{
					color->d_color.r = 0.f;
					color->d_color.g = 1.f;
					color->d_color.b = 0.f;
				}
				if (this->in_walkable_debug && !this->walkable[j * MAX_I + i])
				{
					color->d_color.r = 1.f;
					color->d_color.g = 0.f;
					color->d_color.b = 0.f;
				}

				if (this->in_pos_debug && this->pos[i][j] != -1)
				{
					color->d_color.r = 0.f;
					color->d_color.g = 1.f;
					color->d_color.b = 0.f;
				}
				if (this->in_pos_debug && this->pos[i][j] == -1)
				{
					color->d_color.r = 1.f;
					color->d_color.g = 0.f;
					color->d_color.b = 0.f;
				}
			}
		}
	}
	bool GameBoard::getGridCellFromMouse(s32& out_x, s32& out_y) const
	{
		// 1. Get raw mouse position
		s32 mx, my;
		AEInputGetCursorPosition(&mx, &my);

		// 2. Convert to centered screen space (same as camera system does)
		float sx = float(mx) - float(AEGfxGetWindowWidth()) * 0.5f;
		float sy = -float(my) + float(AEGfxGetWindowHeight()) * 0.5f;

		// 3. Get camera transform
		Components::Transform* camTrans = ecs.getComponent<Components::Transform>(CS.id());
		float zoom = camTrans->size.x; // size.x == size.y == buffer_zoom

		// 4. Unproject: screen → world
		// AEngine renders world as:  screen = (world - camPos) * zoom
		// So:                        world  =  screen / zoom + camPos
		float wx = sx / zoom + camTrans->pos.x;
		float wy = sy / zoom + camTrans->pos.y;

		// 5. Inverse isometric transform (same math as before)
		float rx = wx - offset.x;
		float ry = wy - offset.y;

		float sum = -4.0f * ry / CELL_HEIGHT;  // i + j
		float diff = 2.0f * rx / CELL_WIDTH;   // i - j

		int i = static_cast<int>(std::roundf((sum + diff) * 0.5f));
		int j = static_cast<int>(std::roundf((sum - diff) * 0.5f));

		// 6. Bounds check
		if (i < 0 || i >= MAX_I || j < 0 || j >= MAX_J)
			return false;

		out_x = i;
		out_y = j;
		return true;
	}

	std::array<std::array<Entity, MAX_J>, MAX_I>& GameBoard::get_pos()
	{
		return pos;
	}

	/*std::array<std::array<highlight_tag, MAX_J>, MAX_I>& GameBoard::activate_highlight()
	{
		return highlight_activate;
	}*/

	/*std::array<std::array<int, MAX_J>, MAX_I>& GameBoard::activate_aoe_highlight()
	{
		return aoe_highlight_activate;
	}*/

	bool GameBoard::selected_player() const
	{
		return selected_part;
	}

	void GameBoard::unselect_movement()
	{
		if (!selected_part) return;
		hlptr->unhighlight_mov_cells();

		selected_part = false;
	}

	//returns the grid position of the current participant
	AEVec2 GameBoard::Get_CurPart_gridPos()
	{
		Entity cur_part = tbsptr->current();
		std::array<std::array<Entity, MAX_J>, MAX_I>& positions = get_pos();

		AEVec2 temp_temp_ = { -1.f,-1.f };

		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				if (positions[i][j] == cur_part)
				{
					AEVec2Set(&temp_temp_, (f32)i, (f32)j);
					break;
				}
			}
			if (temp_temp_.x != -1.f && temp_temp_.y != -1.f) break;
		}

		return temp_temp_;
	}

	AEVec2 GameBoard::GetOffsetPos()
	{
		return this->offset;
	}


	bool GameBoard::findEntityCell(Entity e, s32& outX, s32& outY) const
	{
		for (int i = 0; i < MAX_I; ++i)
			for (int j = 0; j < MAX_J; ++j)
				if (pos[i][j] == e)
				{
					outX = i;
					outY = j;
					return true;
				}
		return false;
	}

	void GameBoard::debug_print()
	{
		std::cout << "===BattleGrid===" << '\n';
		std::cout << "Movement Selected : " << selected_part << '\n';
		std::cout << "Previous Pos : " << prev_x << ',' << prev_y << '\n';
		std::cout << "Current Pos : " << cur_x << ',' << cur_y << '\n';
		std::cout << "================" << '\n';
	}

	void  GameBoard::gameboard_free()
	{
		for (Entity& a : cells_entity_id)
		{
			ecs.destroyEntity(a);
		}
		cells_entity_id.clear();

		for (int i = 0; i < int(MAX_I); ++i)
		{
			for (int j = 0; j < int(MAX_J); ++j)
			{
				if (pos[i][j] != -1 && pos[i][j] != playerID)
				{
					ecs.destroyEntity(pos[i][j]);
					pos[i][j] = Entity(-1);
				}
				pos[i][j] = Entity(-1);          // reset ALL cells, not just non-player ones
				cells[i][j] = Entity(-1);        // clear stale cell entity IDs
				walkable[j * int(MAX_I) + i] = uint8_t(-1); // reset walkability
			}
		}

		// reset selection state
		selected_part = false;
		cur = 0;
		prev_cur = 0;
		cur_x = cur_y = 0;
		prev_x = prev_y = 0;
		placementDirection = 0;
		mouse_on_board = false;

		tbsptr = nullptr;
		gbsptr = nullptr;
		evsptr = nullptr;
		cbsptr = nullptr;
		hlptr = nullptr;
	}
	
	bool GameBoard::moveEntityAI(Entity e, s32 x, s32 y, int max_move)
	{
		if (x < 0 || x >= MAX_I || y < 0 || y >= MAX_J)
			return false;

		if (pos[x][y] != -1)
			return false;

		s32 ex, ey;
		if (!findEntityCell(e, ex, ey))
			return false;

		Components::GridCell s{ ex, ey };
		Components::GridCell g{ x, y };

		Components::AStarResult* astar = ecs.getComponent<Components::AStarResult>(e);
		if (!astar)
			return false;

		astar->path = AStar_FindPath_Grid4(MAX_I, MAX_J, walkable, s, g).path;

		if (astar->path.empty())
			return false;

		while (static_cast<int>(astar->path.size()) > max_move)
		{
			astar->path.pop_back();
		}

		if (astar->path.empty())
			return false;

		int new_x = astar->path.back().x;
		int new_y = astar->path.back().y;

		pos[ex][ey] = Entity(-1);
		walkable[ey * MAX_I + ex] = 1;

		pos[new_x][new_y] = e;
		walkable[new_y * MAX_I + new_x] = 0;

		Components::Animation_Actor* anim = ecs.getComponent<Components::Animation_Actor>(e);
		if (anim)
		{
			anim->setType(Components::AnimationType::ENEMY_MOVING);
			gbsptr->set_EnemyPhase(PhaseSystem::EnemyPhase::ENEMY_ANIMATION);
		}

		Components::gridData* gd = ecs.getComponent<Components::gridData>(e);
		if (gd)
		{
			gd->prev_x = gd->x;
			gd->prev_y = gd->y;
			gd->x = new_x;
			gd->y = new_y;
		}

		return true;
	}
	void GameBoard::draw_movement_trail(s32 x, s32 y)
	{
		Components::GridCell s{ s32(cur_x), s32(cur_y) };
		Components::GridCell g{ x, y };
		Components::AStarResult astar = AStar_FindPath_Grid4(MAX_I, MAX_J, this->walkable, s, g);

		for (Components::GridCell cell : astar.path)
		{
			hlptr->move_trail_highlighted_cells.push_back({ cell.x , cell.y });
			hlptr->move_trail_highlight_activate[cell.x][cell.y] = 1;
		}
	}
	void GameBoard::func_aoe_hightlight_cells(s32 x, s32 y)
	{
		Entity card_ID = cbsptr->draw_card(playerID, tbsptr->get_selected_cardhand_index());
		int& aoe_range = ecs.getComponent<Components::Targetting_Component>(card_ID)->aoe;
		int& range = ecs.getComponent<Components::Targetting_Component>(card_ID)->range;

		int serialID = ecs.getComponent<Components::Card_ID>(card_ID)->value;
		bool isManaWall = (serialID == 4220);
		bool isGust = (serialID == 4120);

		AEVec2 cur_part_pos = Get_CurPart_gridPos();
		int rng = grid_dist_manhattan(x, y, (s32)cur_part_pos.x, (s32)cur_part_pos.y);

		//std::cout << "Hovered card serialID = " << serialID << '\n';
		//std::cout << "isManaWall = " << isManaWall << '\n';

		

		if (isGust)
		{
			auto mark_gust = [&](int gx, int gy, int value)
				{
					if (gx < 0 || gx >= MAX_I || gy < 0 || gy >= MAX_J)
						return;

					if (!hlptr->aoe_highlight_activate[gx][gy])
					{
						hlptr->aoe_highlighted_cells.push_back({ gx, gy });
					}

					if (hlptr->aoe_highlight_activate[gx][gy] < value)
					{
						hlptr->aoe_highlight_activate[gx][gy] = value;
					}
				};

			int cx = MAX_I / 2;
			int cy = MAX_J / 2;

			switch (placementDirection)
			{
			case 0: // EAST
			{
				int shaftEnd = MAX_I - 5;

				// shaft
				for (int gx = 0; gx < shaftEnd; ++gx)
				{
					for (int gy = cy - 1; gy <= cy + 1; ++gy)
					{
						mark_gust(gx, gy, 1);
					}
				}

				// head base
				for (int gy = cy - 1; gy <= cy + 1; ++gy)
				{
					mark_gust(shaftEnd, gy, 2);
				}

				// head flare
				for (int gy = cy - 2; gy <= cy + 2; ++gy)
				{
					mark_gust(shaftEnd + 1, gy, 2);
				}

				// narrowing
				for (int gy = cy - 1; gy <= cy + 1; ++gy)
				{
					mark_gust(shaftEnd + 2, gy, 2);
				}

				// tip
				mark_gust(shaftEnd + 3, cy, 3);
				break;
			}

			case 1: // SOUTH
			{
				int shaftEnd = MAX_J - 5;

				// shaft
				for (int gy = 0; gy < shaftEnd; ++gy)
				{
					for (int gx = cx - 1; gx <= cx + 1; ++gx)
					{
						mark_gust(gx, gy, 1);
					}
				}

				// head base
				for (int gx = cx - 1; gx <= cx + 1; ++gx)
				{
					mark_gust(gx, shaftEnd, 2);
				}

				// head flare
				for (int gx = cx - 2; gx <= cx + 2; ++gx)
				{
					mark_gust(gx, shaftEnd + 1, 2);
				}

				// narrowing
				for (int gx = cx - 1; gx <= cx + 1; ++gx)
				{
					mark_gust(gx, shaftEnd + 2, 2);
				}

				// tip
				mark_gust(cx, shaftEnd + 3, 3);
				break;
			}

			case 2: // WEST
			{
				int shaftStart = 4;

				// shaft
				for (int gx = shaftStart; gx < MAX_I; ++gx)
				{
					for (int gy = cy - 1; gy <= cy + 1; ++gy)
					{
						mark_gust(gx, gy, 1);
					}
				}

				// head base
				for (int gy = cy - 1; gy <= cy + 1; ++gy)
				{
					mark_gust(shaftStart - 1, gy, 2);
				}

				// head flare
				for (int gy = cy - 2; gy <= cy + 2; ++gy)
				{
					mark_gust(shaftStart - 2, gy, 2);
				}

				// narrowing
				for (int gy = cy - 1; gy <= cy + 1; ++gy)
				{
					mark_gust(shaftStart - 3, gy, 2);
				}

				// tip
				mark_gust(shaftStart - 4, cy, 3);
				break;
			}

			case 3: // NORTH
			{
				int shaftStart = 4;

				// shaft
				for (int gy = shaftStart; gy < MAX_J; ++gy)
				{
					for (int gx = cx - 1; gx <= cx + 1; ++gx)
					{
						mark_gust(gx, gy, 1);
					}
				}

				// head base
				for (int gx = cx - 1; gx <= cx + 1; ++gx)
				{
					mark_gust(gx, shaftStart - 1, 2);
				}

				// head flare
				for (int gx = cx - 2; gx <= cx + 2; ++gx)
				{
					mark_gust(gx, shaftStart - 2, 2);
				}

				// narrowing
				for (int gx = cx - 1; gx <= cx + 1; ++gx)
				{
					mark_gust(gx, shaftStart - 3, 2);
				}

				// tip
				mark_gust(cx, shaftStart - 4, 3);
				break;
			}

			default:
				break;
			}

			return;
		}
		
		if (rng > range)
			return;

		if (isManaWall)
		{
			int offsets[3][2];

			// 0 = east, 2 = west -> horizontal
			// 1 = south, 3 = north -> vertical
			if (placementDirection % 2 == 0)
			{
				offsets[0][0] = -1; offsets[0][1] = 0;
				offsets[1][0] = 0; offsets[1][1] = 0;
				offsets[2][0] = 1; offsets[2][1] = 0;
			}
			else
			{
				offsets[0][0] = 0; offsets[0][1] = -1;
				offsets[1][0] = 0; offsets[1][1] = 0;
				offsets[2][0] = 0; offsets[2][1] = 1;
			}

			for (int i = 0; i < 3; ++i)
			{
				int nx = x + offsets[i][0];
				int ny = y + offsets[i][1];

				if (nx < 0 || nx >= MAX_I || ny < 0 || ny >= MAX_J)
					continue;

				if (!hlptr->aoe_highlight_activate[nx][ny])
				{
					hlptr->aoe_highlighted_cells.push_back({ nx, ny });
					hlptr->aoe_highlight_activate[nx][ny] = 1;
				}
			}

			hlptr->aoe_highlight_activate[x][y] = 2;
			return;
		}

		for (int i = 0; i <= aoe_range; ++i)
		{
			for (int j = 0; j <= aoe_range; ++j)
			{
				if (i + j <= aoe_range && x + i < MAX_I && y + j < MAX_J)
				{
					if (!hlptr->aoe_highlight_activate[x + i][y + j])
					{
						hlptr->aoe_highlighted_cells.push_back({ x + i, y + j});
						hlptr->aoe_highlight_activate[x + i][y + j] = 1;
					}
				}
				if (i + j <= aoe_range && x - i >= 0 && y - j >= 0)
				{
					if (!hlptr->aoe_highlight_activate[x - i][y - j])
					{
						hlptr->aoe_highlighted_cells.push_back({ x - i, y - j });
						hlptr->aoe_highlight_activate[x - i][y - j] = 1;
					}
				}
				if (i + j <= aoe_range && x + i < MAX_I && y - j >= 0)
				{
					if (!hlptr->aoe_highlight_activate[x + i][y - j])
					{
						hlptr->aoe_highlighted_cells.push_back({ x + i, y - j });
						hlptr->aoe_highlight_activate[x + i][y - j] = 1;
					}
				}
				if (i + j <= aoe_range && x - i >= 0 && y + j < MAX_J)
				{
					if (!hlptr->aoe_highlight_activate[x - i][y + j])
					{
						hlptr->aoe_highlighted_cells.push_back({ x - i, y + j });
						hlptr->aoe_highlight_activate[x - i][y + j] = 1;
					}
				}
			}
		}

		hlptr->aoe_highlight_activate[x][y] = 2;
	}

	void GameBoard::setEnemyAnimationPhase()
	{
		if (gbsptr)
		{
			gbsptr->set_EnemyPhase(PhaseSystem::EnemyPhase::ENEMY_ANIMATION);
		}
	}
}