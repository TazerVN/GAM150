#include "pch.h"

#include "GridSystem.h"
#include <utility> // for std::move
#include "../util/util.h"
#include "../system/TurnBasedSystem.h"
#include "../system/PhaseSystem.h"
#include "../util/Pathfinding.h"
#include "../global.h"
#include "../ECS/Components.h"

float offset = 1.0f;

namespace Grid
{
	void GameBoard::updateCell(EntityComponent::Registry& ecs, s32 x, s32 y)
	{
		//if (is_active)
		//if the current participant ha	s selected card 
				//and selected on the empty cell then return

		if (!(gbsptr->getGBPhase() == PhaseSystem::GBPhase::MAIN_PHASE 
			|| gbsptr->getGBPhase() == PhaseSystem::GBPhase::WIN)) return;
		if (tbsptr->current() != playerID) return;

		switch (gbsptr->getPlayerPhase())
		{
			case PhaseSystem::PlayerPhase::AOE_GRID_SELECT :
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
						std::cout << "Selected cell is outside range" << std::endl;
						PUT << "Out of Range";
						unselect_card();
						return;
					}
					trigger_play_card(ecs, x, y);
				}
				break;
			}
			case PhaseSystem::PlayerPhase::GRID_SELECT :
			{
				if (tbsptr->is_current_selected_card())
				{
					if (pos[x][y] != -1)
					{
						if (pos[x][y] == tbsptr->current())
						{
							unselect_card();
							move_select(x, y);
							return;
						}
						if (!cbsptr->check_within_range(x, y))
						{
							std::cout << "Target is outside range" << std::endl;
							PUT << "Out of range!";
							unselect_card();

							return;
						}
						trigger_play_card(ecs, x,y);
					}
					else {
						std::cout << "Select a valid cell with entity" << std::endl;
						unselect_card();
					}
				}
				break;
			}
			case PhaseSystem::PlayerPhase::PLAYER_EXPLORE:
			{
				//check if an empty cell is clicked
				if (selected_part && this->cur != -1) 
				{
					move_trigger(x,y);
				}
				//check if the grid cell with entity is clicked
				else if (!selected_part && this->pos[x][y] != -1) 
				{
					move_select(x,y);
				}
				break;
			}
			default :
				break;
		}
	}

	void GameBoard::trigger_play_card(EntityComponent::Registry& ecs,s32 x, s32 y)
	{
		cbsptr->set_targetted_ent(pos[x][y]);
		cbsptr->set_targetted_xy(x, y);
		gbsptr->set_GBPhase(PhaseSystem::GBPhase::PLAYER_RESOLUTION);
		gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_ANIMATION);
		gbsptr->GBPTriggered()[static_cast<size_t>(PhaseSystem::GBPhase::PLAYER_RESOLUTION)] = true;

		Components::Animation_Actor* anim = ecs.getComponent<Components::Animation_Actor>(playerID);
		anim->anim_type = Components::AnimationType::ATTACK_MELEE;
	}

	void GameBoard::unselect_card()
	{
		tbsptr->set_selected_card(false);
		gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_EXPLORE);
		evsptr->template_pool[UNHIGHLIGHT_EVENT].triggered = true;
	}

	void GameBoard::move_trigger(s32 const& x, s32 const& y)
	{
		//check if it is within movement range 
		if (ecs.getComponent<Components::TurnBasedStats>(playerID)->cur_movSpd == 0)
		{
			PUT << "Out of stamina";
			unselect_movement();
			return;
		}
		else
		{
			if (!cbsptr->check_within_range(x, y))
			{
				std::cout << "Outside movement range" << std::endl;
				PUT << "Outside range";
				unselect_movement();
				return;
			}

			if (this->pos[x][y] != -1)
			{
				std::cout << "Cannot move onto another entity" << std::endl;
				PUT << "Already Occupied";
				unselect_movement();
				return;
			}
		}
		walkable[cur_x * MAX_J + cur_y] = 1;
		//move the entity
		this->moveEntity(*ecsptr, this->cur, x, y);
		unselect_movement();

		gbsptr->set_GBPhase(PhaseSystem::GBPhase::PLAYER_RESOLUTION);
		gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_ANIMATION);
		gbsptr->GBPTriggered()[static_cast<size_t>(PhaseSystem::GBPhase::PLAYER_RESOLUTION)] = true;

		tbsptr->show_stats(*ecsptr);
	}

	void GameBoard::move_select(s32 const& x, s32 const& y)
	{
		if (pos[x][y] != tbsptr->current())
		{
			std::cout << "Cannot select this entity!" << std::endl;
			PUT<< "Cannot select this Entity";
			return;
		}

		this->unselect_card();

		if (this->activate[x][y] == false)
		{
			this->activate[x][y] = true;
			this->cur = this->pos[x][y];
			this->cur_x = x;
			this->cur_y = y;
			selected_part = true;
			evsptr->template_pool[HIGHLIGHT_EVENT].triggered = true;
			evsptr->template_pool[HIGHLIGHT_EVENT].returned_value = highlight_tag::MOVE_HIGHLIGHT;
		}
	}
	void cell_onHover(EntityComponent::Registry& ecs, Entity id, Entity character)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(id);
		Components::Timer* t = ecs.getComponent<Components::Timer>(id);

		f32 lerp = t->seconds / (t->max_seconds / 2.f) >= 1.f ? t->max_seconds - t->seconds : t->seconds ;
		f32 minimum = 0.6f;

		c->d_color.r = minimum + (1.f - minimum) * lerp;
		c->d_color.b = minimum + (1.f - minimum) * lerp;
		c->d_color.g = minimum + (1.f - minimum) * lerp;
		

		if(character != -1)
		{
			Components::Color* cc = ecs.getComponent<Components::Color>(character);
			cc->d_color.r = minimum + (1.f - minimum) * lerp;
			cc->d_color.b = minimum + (1.f - minimum) * lerp;
			cc->d_color.g = minimum + (1.f - minimum) * lerp;
		
		}
	}

	void cell_offHover(EntityComponent::Registry& ecs, Entity id, Entity character)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(id);
		Components::Timer* t = ecs.getComponent<Components::Timer>(id);

		c->d_color.r = c->c_color.r;
		c->d_color.g = c->c_color.g;
		c->d_color.b = c->c_color.b;
		t->seconds = 0.5f;

		if (character != -1)
		{
			Components::Color* cc = ecs.getComponent<Components::Color>(character);
			cc->d_color.r = cc->c_color.r;
			cc->d_color.g = cc->c_color.g;
			cc->d_color.b = cc->c_color.b;
		}


	}

	


	Entity GameBoard::create_cells(EntityComponent::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, f32 rotation, AEGfxTexture* pTex, s32 x, s32 y, s8 z)
	{
		Entity id = ecs.createEntity();

		Components::Transform trans{ pos, pos, size, {size.x / 2, size.y / 2}, 0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Color color{1.0f, 1.0f, 1.0f ,1.0f};
		Components::Texture texture{ pTex };
		Components::Input in
		(
			AEVK_LBUTTON, true,
			[x, y, this, &ecs]
			{
				this->updateCell(ecs, x, y); 
			}, 
			[x, y,id, &ecs, this]
			{ 
				cell_onHover(ecs, id, this->pos[x][y]);

				if (gbsptr->getGBPhase() != PhaseSystem::GBPhase::PLAYER_RESOLUTION)
				{
					for (AEVec2 a : cbsptr->get_aoe_selected_cell())
					{
						aoe_highlight_activate[int(a.x)][int(a.y)] = 0;
					}
					cbsptr->get_aoe_selected_cell().clear();
				}

				if (gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::AOE_GRID_SELECT)
				{
					Entity card_ID = tbsptr->draw_card(ecs, tbsptr->current(), tbsptr->get_selected_cardhand_index());
					f32& aoe_range = ecs.getComponent<Components::Targetting_Component>(card_ID)->aoe;
					f32& range = ecs.getComponent<Components::Targetting_Component>(card_ID)->range;

					AEVec2 cur_part_pos = Get_CurPart_gridPos();

					int rng = grid_dist_manhattan(x, y, cur_part_pos.x, cur_part_pos.y);

					if (rng <= range)
					{
						for (int i = 0; i <= aoe_range; ++i)
						{
							for (int j = 0; j <= aoe_range; ++j)
							{
								if (i + j <= aoe_range && x + i < MAX_I && y + j < MAX_J)
								{
									if (this->aoe_highlight_activate[x + i][y + j])
										continue;
									cbsptr->get_aoe_selected_cell().push_back({f32(x + i) , f32(y + j)});
									this->aoe_highlight_activate[x + i][y + j] = 1;
								}
								if (i + j <= aoe_range && x - i >= 0 && y - j >= 0)
								{
									if (this->aoe_highlight_activate[x - i][y - j])
										continue;
									cbsptr->get_aoe_selected_cell().push_back({ f32(x - i) , f32(y - j) });
									this->aoe_highlight_activate[x - i][y - j] = 1;
								}
								if (i + j <= aoe_range && x + i < MAX_I && y - j >= 0)
								{
									if (this->aoe_highlight_activate[x + i][y - j])
										continue;
									cbsptr->get_aoe_selected_cell().push_back({ f32(x + i) , f32(y - j) });
									this->aoe_highlight_activate[x + i][y - j] = 1;
								}
								if (i + j <= aoe_range && f32(x - i) >= 0 && f32(y + j) < MAX_J)
								{
									if (this->aoe_highlight_activate[x - i][y + j])
										continue;
									cbsptr->get_aoe_selected_cell().push_back({ f32(x - i) , f32(y + j) });
									this->aoe_highlight_activate[x - i][y + j] = 1;
								}
							}
						}
						this->aoe_highlight_activate[x][y] = 2;
					}
				}
			},
			[x, y, id, &ecs, this]
			{ 
				cell_offHover(ecs, id, this->pos[x][y]);
			}
		);	//add input system for grid
		Components::GridCell gc{ x,y };
		Components::Timer timer{ 1.f , 0.5f, true, true};

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
	void GameBoard::init(EntityComponent::Registry& ecs, MeshFactory& mf, TBS::TurnBasedSystem* tbsys, EventPool<highlight_tag>& evs, PhaseSystem::GameBoardState& gb,
		CombatNameSpace::CombatSystem& cbs, AEGfxTexture* pTex, f32 ox, f32 oy, bool& _win)
	{
		tbsptr = tbsys;
		ecsptr = &ecs;
		evsptr = &evs;
		gbsptr = &gb;
		cbsptr = &cbs;

		win = &_win;
		this->offset.x = ox;
		this->offset.y = oy;
		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				f32 x = this->offset.x + (i - j) * CELL_WIDTH / 2; /*+ offset*//*if offset is required*/
				f32 y = this->offset.y - (i + j) * CELL_HEIGHT / 4; /*+ offset*//*if offset is required*/

				cells[i][j] = create_cells(ecs, mf, { x,y }, { 128.f,128.f }, 0.f, pTex, i, j, 0);
				this->pos[i][j] = -1;
				this->activate[i][j] = false;
				this->highlight_activate[i][j] = highlight_tag::UNHIGHLIGHTED;
				walkable[j * MAX_I + i] = 1;
			}
		}
	}

	void GameBoard::placeEntity(EntityComponent::Registry& ecs, Entity e, s32 x, s32 y)
	{
		if (x >= MAX_I || x < 0 || y >= MAX_J || y < 0) return;

		this->pos[x][y] = e;
		this->walkable[x * MAX_J + y] = 0;

		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
		if (!ecs.getBitMask()[e].test(transID)) return;

		Components::Transform* transform = ecs.getComponent<Components::Transform>(e);
		Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(e);

		transform->pos.x = this->offset.x + (x - y) * CELL_WIDTH / 2;
		transform->pos.y = transform->size.y / 3 + this->offset.y - (x + y) * CELL_HEIGHT / 4;
		transform->pos_onscreen = transform->pos;
		mesh->z = x + 2;


		Entity current_cell = this->cells[x][y];
		EntityComponent::ComponentTypeID colorID = EntityComponent::getComponentTypeID<Components::Color>();
		if (!ecs.getBitMask()[current_cell].test(colorID)) return;

		Components::Color* color = ecs.getComponent<Components::Color>(current_cell);
		color->d_color.g = 0.5f;
		color->d_color.r = 0.5f;
	}

	void GameBoard::moveEntity(EntityComponent::Registry& ecs, Entity e, s32 x, s32 y)
	{
		int start_i{}, start_j{};
		//get player's position first
		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				if (this->pos[i][j] == e)
				{
					start_i = i;
					start_j = j;
					this->pos[i][j] = -1;

					int dist = grid_dist_manhattan(i, j, x, y);

					if (!*win)
					{
						ecs.getComponent<Components::TurnBasedStats>(e)->cur_movSpd -= dist;
					}
				}
			}
		}

		EntityComponent::ComponentTypeID astarID = EntityComponent::getComponentTypeID<Components::AStarResult>();
		EntityComponent::ComponentTypeID animID = EntityComponent::getComponentTypeID<Components::Animation_Actor>();

		if (ecs.getBitMask()[e].test(astarID)){

			Components::GridCell s{ start_i, start_j };
			Components::GridCell g{ x,  y };

			Components::AStarResult* astar = ecs.getComponent<Components::AStarResult>(e);
			astar->path = AStar_FindPath_Grid4(MAX_I, MAX_J, walkable, s, g).path;
		}

		if (ecs.getBitMask()[e].test(animID))
		{
			Components::Animation_Actor* anim = ecs.getComponent<Components::Animation_Actor>(e);
			anim->anim_type = Components::AnimationType::MOVING;
			
		}

		this->pos[x][y] = e;
		this->walkable[x * MAX_J + y] = 0;
	}

	void GameBoard::update(EntityComponent::Registry& ecs,Entity camera)
	{
		if (tbsptr->is_current_selected_card()
		 && AEInputCheckTriggered(AEVK_RBUTTON))
		{
			gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_EXPLORE);
			tbsptr->set_selected_card(false);
			evsptr->template_pool[UNHIGHLIGHT_EVENT].triggered = true;
			//gbsptr->debug_print();
		}
		if (selected_part && AEInputCheckTriggered(AEVK_RBUTTON))
		{
			unselect_movement();
			evsptr->template_pool[UNHIGHLIGHT_EVENT].triggered = true;
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

				switch (this->highlight_activate[i][j])
				{
				case highlight_tag::ATTACK_HIGHLIGHT:
				{
					color->d_color.r = color->d_color.r + 0.5f;
					color->d_color.g = color->d_color.g - 0.3f;
					color->d_color.b = color->d_color.b - 0.3f;
					break;
				}
				case highlight_tag::MOVE_HIGHLIGHT :
				{
					color->d_color.r = color->d_color.r - 0.2f;
					color->d_color.g = color->d_color.g - 0.2f;
					color->d_color.b = color->d_color.b + 0.4f;
					break;
				}
				default:
					break;
				}


				if(this->aoe_highlight_activate[i][j])
				{
					color->d_color.r -= (aoe_highlight_activate[i][j] == 2) ? 0.6f : 0.4;
					color->d_color.g -= 0.8f;
					color->d_color.b -= 0.8f;
				}

				//update entity cell
				if (this->pos[i][j] == -1) 
				{ 
					continue;
				}

				Entity e = this->pos[i][j];

				if (!ecs.getBitMask()[e].test(transID)) return;
				if (!ecs.getBitMask()[e].test(colorID)) return;

				transform = ecs.getComponent<Components::Transform>(e);
				color = ecs.getComponent<Components::Color>(e);

				/*transform->pos.x = this->offset.x + (i - j) * CELL_WIDTH / 2;
				transform->pos.y = transform->size.y / 2 + this->offset.y - (i + j) * CELL_HEIGHT / 4;
				transform->pos.y = transform->size.y / 3 + this->offset.y - (i + j) * CELL_HEIGHT / 4;
				transform->pos_onscreen = transform->pos;*/

				
				Entity current_cell = this->cells[i][j];
				colorID = EntityComponent::getComponentTypeID<Components::Color>();
				if (!ecs.getBitMask()[current_cell].test(colorID)) return;

				color = ecs.getComponent<Components::Color>(current_cell);
				color->d_color.g = color->d_color.g - 0.4f;
				color->d_color.r = color->d_color.r - 0.4f;
			}
		}
	}

	std::array<std::array<Entity, MAX_J>, MAX_I>& GameBoard::get_pos()
	{
		return pos;
	}

	std::array<std::array<highlight_tag, MAX_J>, MAX_I>& GameBoard::activate_highlight()
	{
		return highlight_activate;
	}

	std::array<std::array<int, MAX_J>, MAX_I>& GameBoard::activate_aoe_highlight()
	{
		return aoe_highlight_activate;
	}

	bool GameBoard::selected_player() const
	{
		return selected_part;
	}

	void GameBoard::unselect_movement()
	{
		if (!selected_part) return;
		evsptr->template_pool[UNHIGHLIGHT_EVENT].triggered = true;
		selected_part = false;
		this->activate[this->cur_x][this->cur_y] = false;
		this->cur_x = -1;
		this->cur_y = -1;
		this->cur = -1;
	}

	AEVec2 GameBoard::Get_gridPos(AEVec2 const& pos,Entity camera)
	{
		// 2. AE screen space: origin is CENTER of screen, Y flips upward
		f32 winW = (f32)AEGfxGetWindowWidth();
		f32 winH = (f32)AEGfxGetWindowHeight();

		f32 screenX = (f32)pos.x - winW * 0.5f;
		f32 screenY = -(f32)pos.y + winH * 0.5f;   // flip Y
		// 3. Add camera offset to get world position
		Components::Transform* cam = ecsptr->getComponent<Components::Transform>(camera);
		f32 worldX = screenX + cam->pos.x;
		f32 worldY = screenY + cam->pos.y;

		// 4. Inverse isometric formula
		// forward: worldX = offset.x + (i - j) * CELL_WIDTH  / 2
		//          worldY = offset.y - (i + j) * CELL_HEIGHT / 4
		f32 sx = worldX - this->offset.x;
		f32 sy = worldY - this->offset.y;

		f32 diff = 2.0f * sx / CELL_WIDTH;    // i - j
		f32 sum = -4.0f * sy / CELL_HEIGHT;   // i + j

		f32 i = (sum + diff) / 2.0f;
		f32 j = (sum - diff) / 2.0f;

		// 5. Clamp to grid bounds
		i = AEClamp(std::round(i), 0, MAX_I - 1);
		j = AEClamp(std::round(j), 0, MAX_J - 1);

		return { f32(math_absolute(i)), f32(math_absolute(j)) };
	}

	//returns the grid position of the current participant
	AEVec2& GameBoard::Get_CurPart_gridPos()
	{
		Entity cur_part = tbsptr->current();
		std::array<std::array<Entity, MAX_J>, MAX_I>& positions = get_pos();

		AEVec2 temp = { -1.f,-1.f };

		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				if (positions[i][j] == cur_part)
				{
					AEVec2Set(&temp, (f32)i, (f32)j);
					break;
				}
			}
			if (temp.x != -1.f && temp.y != -1.f) break;
		}

		return temp;
	}

	AEVec2 GameBoard::GetOffsetPos()
	{
		return this->offset;
	}

	s32 GameBoard::grid_dist_manhattan(s32 const& x1, s32 const& y1, s32 const& x2, s32 const& y2)
	{
		return math_absolute(x2 - x1) + math_absolute(y2 - y1);
	}

	s32 GameBoard::grid_dist_chebyshev(s32 const& x1, s32 const& y1, s32 const& x2, s32 const& y2)
	{
		int lhs{ math_absolute(x2 - x1) }; int rhs{ math_absolute(y2 - y1) };
		return math_max( lhs,rhs );
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

	void  GameBoard::gameboard_free()
	{
		for (Entity a : cells_entity_id)
		{
			ecs.destroyEntity(a);
		}
		cells_entity_id.clear();
		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				if (pos[i][j] != -1 && pos[i][j] != playerID)
				{
					ecs.destroyEntity(pos[i][j]);
				}
			}
		}
		aoe_highlighted_cells.clear();
		win = nullptr;
	}
	/*bool GameBoard::moveEntityAI(EntityComponent::Registry& ecs, Entity e, s32 x, s32 y)
	{
		if (x < 0 || x >= MAX_I || y < 0 || y >= MAX_J)
			return false;

		if (pos[x][y] != -1)
			return false;

		s32 ex, ey;
		if (!findEntityCell(e, ex, ey))
			return false;

		pos[ex][ey] = -1;

		pos[x][y] = e;

		return true;
	}*/
	//temporary fix so enemies could folow the tile they are meant to follow.
	bool GameBoard::moveEntityAI(EntityComponent::Registry& ecs, Entity e, s32 x, s32 y)
	{
		if (x < 0 || x >= MAX_I || y < 0 || y >= MAX_J)
			return false;

		if (pos[x][y] != -1)
			return false;

		s32 ex, ey;
		if (!findEntityCell(e, ex, ey))
			return false;

		pos[ex][ey] = -1;
		walkable[ex * MAX_J + ey] = 1;

		pos[x][y] = e;
		walkable[x * MAX_J + y] = 0;

		EntityComponent::ComponentTypeID transID =
			EntityComponent::getComponentTypeID<Components::Transform>();

		if (ecs.getBitMask()[e].test(transID))
		{
			Components::Transform* transform = ecs.getComponent<Components::Transform>(e);
			Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(e);

			transform->pos.x = this->offset.x + (x - y) * CELL_WIDTH / 2;
			transform->pos.y = transform->size.y / 3 + this->offset.y - (x + y) * CELL_HEIGHT / 4;
			transform->pos_onscreen = transform->pos;
			mesh->z = x + 2;
		}

		return true;
	}

}