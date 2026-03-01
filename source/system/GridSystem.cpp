#include "GridSystem.h"
#include <utility> // for std::move
#include "../util/util.h"
#include "../system/TurnBasedSystem.h"
#include "../system/PhaseSystem.h"

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
		//if the current participant has selected card 
				//and selected on the empty cell then return
		if (tbs->is_current_selected_card())
		{
			//if the card is selected and the selected pos is entity
			if (pos[x][y] != -1 && pos[x][y] != tbs->current())
			{	
				//determine target 
				Entity target = pos[x][y];
				Entity current_entt = tbs->current();
				Entity cardID = tbs->draw_card(ecs, current_entt, tbs->get_selected_cardhand_index());
				bool died = tbs->play_card(ecs, target, cardID);
				if (died)
				{
					if (x != -1 && y != -1) pos[x][y] = -1;
					tbs->remove_participant(ecs, target);
				}
				tbs->set_selected_card(false);
				evsptr->pool[UNHIGHLIGHT_EVENT].triggered = true;
				return;
			}
			else {
				tbs->set_selected_card(false);
				evsptr->pool[UNHIGHLIGHT_EVENT].triggered = true;
				std::cout << "Select a valid cell with entity!" << std::endl;
			}
			return;
		}

		//check if an empty cell is clicked
		if (this->cur != -1) {
			this->moveEntity(ecs, this->cur, x, y);
			this->activate[x][y] = false;
			this->activate[this->cur_x][this->cur_y] = false;
			this->cur_x = -1;
			this->cur_y = -1;
			this->cur = -1;
		}
		//check if the grid cell with entity is clicked
		else if (this->pos[x][y] != -1) {

			//check if the entity is the same as the current turn
				//if the current turn is not the entity then dont allow selection

			if (!tbs->is_current_selected_card() && pos[x][y] != tbs->current())
			{
				std::cout << "Cannot select this entity!" << std::endl;
				return;
			}

			if (this->activate[x][y] == false) {
				this->activate[x][y] = true;
				this->cur = this->pos[x][y];
				this->cur_x = x;
				this->cur_y = y;
			}

			else {
				this->activate[x][y] = false;
				this->cur = -1;
				this->cur_x = -1;
				this->cur_y = -1;
			}
		}
	}


	Entity GameBoard::create_cells(ECS::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, f32 rotation, AEGfxTexture* pTex, s32 x, s32 y, s8 z)
	{
		Entity id = ecs.createEntity();

		Components::Transform trans{ pos, pos, size, {size.x / 2, size.y / 2}, 0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Color color{1.0f, 1.0f, 1.0f ,1.0f};
		Components::Texture texture{ pTex };
		Components::Input in( AEVK_LBUTTON, true, [x, y, this, &ecs] {this->updateCell(ecs, x, y);}, nullptr, nullptr);	//add input system for grid

		Components::GridCell gc{ x,y };

		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);
		ecs.addComponent(id, gc);
		ecs.addComponent(id, in);

		return id;
	}
	void GameBoard::init(ECS::Registry& ecs, MeshFactory& mf, TBS::TurnBasedSystem* tbsys, EventPool& evs, PhaseSystem::GameBoardState& gb, AEGfxTexture* pTex, f32 ox, f32 oy)
	{
		tbs = tbsys;
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
				this->atk_activate[i][j] = false;
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
		color->p_color.g = 0.5f;
		color->p_color.r = 0.5f;
	}

	void GameBoard::moveEntity(ECS::Registry& ecs, Entity e, s32 x, s32 y)
	{
		//check if there is an entity on the selected tile first
		if (this->pos[x][y] != -1) return;

		bool isHere = false;
		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				if (this->pos[i][j] == e)
				{
					this->pos[i][j] = -1;
					isHere = true;
				}
			}
		}

		if (isHere == false) return;

		this->pos[x][y] = e;

		//whenever entity moves advances turn order
		//tbs->next(ecs);
	}

	void GameBoard::update(ECS::Registry& ecs)
	{

		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				//update all cells
				ECS::ComponentTypeID transID = ECS::getComponentTypeID<Components::Transform>();
				ECS::ComponentTypeID colorID = ECS::getComponentTypeID<Components::Color>();

				Components::Transform* transform = ecs.getComponent<Components::Transform>(this->cells[i][j]);
				Components::Color* color = ecs.getComponent<Components::Color>(this->cells[i][j]);


				if (!this->atk_activate[i][j])
				{
					color->p_color = color->c_color;
				}

				if (this->activate[i][j])
				{
					color->p_color.b = 0.5f;
					color->p_color.r = 0.5f;
				}
				else{
					color->p_color = color->c_color;
					
				}

				if (this->atk_activate[i][j])
				{
					color->p_color.r = 1.f;
					color->p_color.g = 0.f;
					color->p_color.b = 0.f;
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
				color->p_color.g = 0.5f;
				color->p_color.r = 0.5f;
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

	std::array<std::array<bool, MAX_J>, MAX_I>& GameBoard::get_attack_activate()
	{
		return atk_activate;
	}

	AEVec2 GameBoard::Get_gridPos(AEVec2 const& worldPos)
	{
		float rel_x = worldPos.x - this->offset.x;
		float rel_y = worldPos.y - this->offset.y;

		int i = (int)(rel_x / (CELL_WIDTH / 2) - rel_y / (CELL_HEIGHT / 4)) / 2;
		int j = (int)(-rel_x / (CELL_WIDTH / 2) - rel_y / (CELL_HEIGHT / 4)) / 2;

		if (i < 0 || i >= MAX_I || j < 0 || j >= MAX_J) return { -1.f, -1.f };

		return { f32(i), f32(j) };
	}
}