#include "GridSystem.h"
#include <utility> // for std::move
#include "../util/util.h"
#include "../system/TurnBasedSystem.h"
#include "../system/PhaseSystem.h"
#include "../util/Pathfinding.h"
#include "../global.h"

float offset = 1.0f;

namespace Grid
{
	//// helper: pick nearest cell by distance to its center
	//bool Grid::PickCellNearest(float worldX, float worldY, int& outI, int& outJ) const
	//{
	//	float mx = (float)worldX;
	//	float my = (float)worldY;

	//	float bestD2 = 1e30f;
	//	bool found = false;

	//	for (int i = 0; i < MAX_I; ++i)
	//	{
	//		for (int j = 0; j < MAX_J; ++j)
	//		{
	//			// CHANGE these if your Rectangle stores position differently
	//			float cx = cells[i][j].pos.x;
	//			float cy = cells[i][j].pos.y;

	//			float dx = mx - cx;
	//			float dy = my - cy;
	//			float d2 = dx * dx + dy * dy;

	//			if (d2 < bestD2)
	//			{
	//				bestD2 = d2;
	//				outI = i;
	//				outJ = j;
	//				found = true;
	//			}
	//		}
	//	}

	//	return found;
	//}

	//void Grid::RebuildWalkable()
	//{
	//	for (int j = 0; j < MAX_J; ++j)
	//	{
	//		for (int i = 0; i < MAX_I; ++i)
	//		{
	//			walkable[j * MAX_I + i] = blocked[i][j] ? 0 : 1;
	//		}
	//	}
	//}

	//void Grid::ResetColors()
	//{
	//	for (int i = 0; i < MAX_I; ++i)
	//	{
	//		for (int j = 0; j < MAX_J; ++j)
	//		{
	//			cells_alpha[i][j] = 1.0f;
	//			cells_color[i][j] = blocked[i][j] ? 0x222222 : 0xFFFFFF; //BLOCKED = BLACK
	//		}
	//	}
	//}


	//void Grid::PaintStartGoalPath()
	//{
	//	// path cyan
	//	for (const Cell& c : path)
	//	{
	//		if (c.x < 0 || c.x >= MAX_I || c.y < 0 || c.y >= MAX_J) continue;
	//		cells_color[c.x][c.y] = 0x00FFFF;
	//	}

	//	// start green, goal red override
	//	if (hasStart) cells_color[start_i][start_j] = 0x00FF00;
	//	if (hasGoal)  cells_color[goal_i][goal_j] = 0xFF0000;
	//}

	//void Grid::ComputePath()
	//{
	//	path.clear();
	//	RebuildWalkable();

	//	if (!hasStart || !hasGoal) return;
	//	if (blocked[start_i][start_j] || blocked[goal_i][goal_j]) return;

	//	Cell s{ start_i, start_j };
	//	Cell g{ goal_i,  goal_j };

	//	AStarResult res = AStar_FindPath_Grid4(MAX_I, MAX_J, walkable, s, g);
	//	path = std::move(res.path);
	//}

	//void Grid::ToggleBlocked(int i, int j)
	//{
	//	if (i < 0 || i >= MAX_I || j < 0 || j >= MAX_J) return;

	//	// If we're blocking/unblocking on start/goal, remove the marker first
	//	if (hasStart && i == start_i && j == start_j)
	//	{
	//		hasStart = false;
	//		start_i = start_j = -1;
	//		path.clear();
	//	}

	//	if (hasGoal && i == goal_i && j == goal_j)
	//	{
	//		hasGoal = false;
	//		goal_i = goal_j = -1;
	//		path.clear();
	//	}

	//	blocked[i][j] = !blocked[i][j];
	//}

	//Grid::Grid()
	//{
	//	for (int i = 0; i < MAX_I; ++i)
	//	{
	//		for (int j = 0; j < MAX_J; ++j)
	//		{
	//			float cell_x = i * ((float)CELL_WIDTH + offset);
	//			float cell_y = -(j * ((float)CELL_HEIGHT + offset));


	//			float display_x = (i - j) * CELL_WIDTH / 2;
	//			float display_y = -(i + j) * CELL_HEIGHT / 4;
	//			cells[i][j] = Shape2D::Rectangle(display_x, display_y, CELL_WIDTH, CELL_HEIGHT);
	//			cells_color[i][j] = 0xFFFFFF;
	//			cells_alpha[i][j] = 1.0f;
	//		}
	//	}
	//	// init walkable (no walls yet)
	//	RebuildWalkable();
	//	ResetColors();
	//}


	//void Grid::update(float camX, float camY, int winW, int winH)
	//{
	//	int mouseX, mouseY;
	//	AEInputGetCursorPosition(&mouseX, &mouseY);

	//	float worldX = camX + ((float)mouseX - winW * 0.5f);
	//	float worldY = camY + (winH * 0.5f - (float)mouseY);

	//	// Toggle block on the tile under the mouse when B is pressed
	//	if (AEInputCheckTriggered(AEVK_B))
	//	{
	//		int i, j;
	//		if (PickCellNearest(worldX, worldY, i, j))
	//		{
	//			ToggleBlocked(i, j);

	//			if (hasStart && hasGoal)
	//				ComputePath();

	//			ResetColors();
	//			PaintStartGoalPath();
	//		}
	//	}



	//	if (AEInputCheckTriggered(AEVK_LBUTTON))
	//	{
	//		int i, j;

	//		if (PickCellNearest(worldX, worldY, i, j))
	//		{
	//			if (blocked[i][j]) return; //dont select blocked tile

	//			// Click 1: set start
	//			if (!hasStart || (hasStart && hasGoal))
	//			{
	//				hasStart = true;
	//				hasGoal = false;
	//				start_i = i; start_j = j;
	//				goal_i = goal_j = -1;
	//				path.clear();
	//			}
	//			// Click 2: set goal, compute path
	//			else
	//			{
	//				hasGoal = true;
	//				goal_i = i; goal_j = j;

	//				ComputePath();
	//			}

	//			// recolor every click
	//			ResetColors();
	//			PaintStartGoalPath();
	//		}
	//	}


	//}





	/*void Grid::render()
	{
		if (this->mode == TEXTURE)
		{
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			AEGfxSetBlendMode(AE_GFX_BM_BLEND);
			AEGfxTextureSet(this->texture, 0, 0);
			AEGfxSetTransparency(1.0f);
		}
		else
		{
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		}
		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				Color::Color_Fill_Hex(cells_color[i][j], cells_alpha[i][j]);
				AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
				cells[i][j].m_mode = this->m_mode;
				cells[i][j].mode = this->mode;
				cells[i][j].render();
			}
		}
	}*/


	void GameBoard::updateCell(ECS::Registry& ecs, s32 x, s32 y)
	{
		//if (is_active)
		//if the current participant ha	s selected card 
				//and selected on the empty cell then return

		if (!(gbsptr->getGBPhase() == PhaseSystem::GBPhase::START_PHASE || gbsptr->getGBPhase() == PhaseSystem::GBPhase::MAIN_PHASE)) return;
		if (tbsptr->current() != playerID) return;

		switch (gbsptr->getPlayerPhase())
		{
			case PhaseSystem::PlayerPhase::AOE_GRID_SELECT :
			case PhaseSystem::PlayerPhase::GRID_SELECT :
			{
				if (tbsptr->is_current_selected_card())
				{
					//if the card is selected and the selected pos has entity
					if (pos[x][y] != -1)
					{
						tbsptr->set_targetted_ent(pos[x][y]);
						tbsptr->set_targetted_xy(x, y);

						tbsptr->play_card_triggered = true;
						gbsptr->set_GBPhase(PhaseSystem::GBPhase::PLAYER_RESOLUTION);
						gbsptr->GBPTriggered()[static_cast<size_t>(gbsptr->getGBPhase())] = true;
					}
					else {
						std::cout << "Select a valid cell with entity" << std::endl;
						tbsptr->set_selected_card(false);
						gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_EXPLORE);
						evsptr->template_pool[UNHIGHLIGHT_EVENT].triggered = true;
					}
				}
				break;
			}
			case PhaseSystem::PlayerPhase::PLAYER_EXPLORE:
			{
				//check if an empty cell is clicked
				if (selected_part && this->cur != -1) 
				{
					//check if it is within movement range 
					if (!check_within_range(this->cur, x, y))
					{
						std::cout << "Outside movement range" << std::endl;
						reset_selected_player();
						evsptr->template_pool[UNHIGHLIGHT_EVENT].triggered = true;
						return;
					}

					if (this->pos[x][y] != -1)
					{
						std::cout << "Cannot move onto another entity" << std::endl;
						return;
					}
					evsptr->template_pool[UNHIGHLIGHT_EVENT].triggered = true;

					//move the entity
					this->moveEntity(ecs, this->cur, x, y);
					reset_selected_player();
					tbsptr->show_stats(ecs);
				}

				//check if the grid cell with entity is clicked
				else if (!selected_part && this->pos[x][y] != -1) {

					//check if the entity is the same as the current turn
						//if the current turn is not the entity then dont allow selection

					if (pos[x][y] != tbsptr->current())
					{
						std::cout << "Cannot select this entity!" << std::endl;
						return;
					}

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
				break;
			}
			default :
				break;
		}
	}

	void cell_onHover(ECS::Registry& ecs, Entity id)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(id);
		c->d_color.r = 0.7f;
		c->d_color.g = 0.7f;
		c->d_color.b = 0.7f;
	}

	void cell_offHover(ECS::Registry& ecs, Entity id)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(id);
		c->d_color.r = c->c_color.r;
		c->d_color.g = c->c_color.g;
		c->d_color.b = c->c_color.b;
	}
	


	Entity GameBoard::create_cells(ECS::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, f32 rotation, AEGfxTexture* pTex, s32 x, s32 y, s8 z)
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
				cell_onHover(ecs, id);

				if(!aoe_highlighted_cells.empty())
					for (AEVec2 a : aoe_highlighted_cells)
					{
						aoe_highlight_activate[int(a.x)][int(a.y)] = 0;
					}

				aoe_highlighted_cells.clear();
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
									this->aoe_highlighted_cells.push_back({ f32(x + i) , f32(y + j) });
									this->aoe_highlight_activate[x + i][y + j] = 1;
								}
								if (i + j <= aoe_range && x - i >= 0 && y - j >= 0)
								{
									this->aoe_highlighted_cells.push_back({ f32(x - i) , f32(y - j) });
									this->aoe_highlight_activate[x - i][y - j] = 1;
								}
								if (i + j <= aoe_range && x + i < MAX_I && y - j >= 0)
								{
									this->aoe_highlighted_cells.push_back({ f32(x + i) , f32(y - j) });
									this->aoe_highlight_activate[x + i][y - j] = 1;
								}
								if (i + j <= aoe_range && f32(x - i) >= 0 && f32(y + j) < MAX_J)
								{
									this->aoe_highlighted_cells.push_back({ f32(x - i) , f32(y + j) });
									this->aoe_highlight_activate[x - i][y + j] = 1;
								}
							}
						}
						this->aoe_highlight_activate[x][y] = 2;
					}
				}
			}, 
			[id, &ecs] 
			{ 
				cell_offHover(ecs, id);
			}
		);	//add input system for grid
		Components::GridCell gc{ x,y };

		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);
		ecs.addComponent(id, gc);
		ecs.addComponent(id, in);

		return id;
	}
	void GameBoard::init(ECS::Registry& ecs, MeshFactory& mf, TBS::TurnBasedSystem* tbsys, EventPool<highlight_tag>& evs, PhaseSystem::GameBoardState& gb, AEGfxTexture* pTex, f32 ox, f32 oy)
	{
		tbsptr = tbsys;
		ecsptr = &ecs;
		evsptr = &evs;
		gbsptr = &gb;

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
			}
		}
	}

	void GameBoard::placeEntity(ECS::Registry& ecs, Entity e, s32 x, s32 y)
	{
		if (x >= MAX_I || x < 0 || y >= MAX_J || y < 0) return;

		this->pos[x][y] = e;

		ECS::ComponentTypeID transID = ECS::getComponentTypeID<Components::Transform>();
		if (!ecs.getBitMask()[e].test(transID)) return;

		Components::Transform* transform = ecs.getComponent<Components::Transform>(e);
		transform->pos.x = this->offset.x + (x - y) * CELL_WIDTH / 2;
		transform->pos.y = transform->size.y / 2 + this->offset.y - (x + y) * CELL_HEIGHT / 4;
		transform->pos_onscreen = transform->pos;


		Entity current_cell = this->cells[x][y];
		ECS::ComponentTypeID colorID = ECS::getComponentTypeID<Components::Color>();
		if (!ecs.getBitMask()[current_cell].test(colorID)) return;

		Components::Color* color = ecs.getComponent<Components::Color>(current_cell);
		color->d_color.g = 0.5f;
		color->d_color.r = 0.5f;
	}

	void GameBoard::moveEntity(ECS::Registry& ecs, Entity e, s32 x, s32 y)
	{
		//get player's position first
		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				if (this->pos[i][j] == e)
				{
					this->pos[i][j] = -1;

					int dist = grid_dist_manhattan(i, j, x, y);
					ecs.getComponent<Components::TurnBasedStats>(e)->cur_movSpd -= dist;
				}
			}
		}
		this->pos[x][y] = e;
	}

	void GameBoard::update(ECS::Registry& ecs,Entity camera)
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
			reset_selected_player();
			evsptr->template_pool[UNHIGHLIGHT_EVENT].triggered = true;
		}
		
		
		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				//update all cells
				ECS::ComponentTypeID transID = ECS::getComponentTypeID<Components::Transform>();
				ECS::ComponentTypeID colorID = ECS::getComponentTypeID<Components::Color>();

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
					color->d_color.r = (aoe_highlight_activate[i][j] == 2) ? 0.5f : 1.f;
					color->d_color.g = 0.f;
					color->d_color.b = 0.f;
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
				transform->pos.x = this->offset.x + (i - j) * CELL_WIDTH / 2;
				transform->pos.y = transform->size.y / 2 + this->offset.y - (i + j) * CELL_HEIGHT / 4;
				transform->pos_onscreen = transform->pos;


				
				Entity current_cell = this->cells[i][j];
				colorID = ECS::getComponentTypeID<Components::Color>();
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

	std::vector<AEVec2>& GameBoard::get_highlighted_cell()
	{
		return highlighted_cells;
	}

	std::array<std::array<highlight_tag, MAX_J>, MAX_I>& GameBoard::activate_highlight()
	{
		return highlight_activate;
	}

	bool GameBoard::selected_player() const
	{
		return selected_part;
	}

	void GameBoard::reset_selected_player()
	{
		if (!selected_part) return;
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

	bool GameBoard::check_within_range(Entity id, s32 const& x, s32 const& y)
	{
		for (AEVec2 ite : highlighted_cells)
		{
			if (x == s32(ite.x) && y == s32(ite.y))
			{
				return true;
			}
		}
		return false;
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

	bool GameBoard::moveEntityAI(Entity e, s32 x, s32 y)
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
	}

}