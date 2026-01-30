#include "Grid.h"
#include "Shape2D.h"
#include <utility>         // for std::move

float offset = 1.0f;

namespace Grid
{
	// helper: pick nearest cell by distance to its center
	bool Grid::PickCellNearest(float worldX, float worldY, int& outI, int& outJ) const
	{
		float mx = (float)worldX;
		float my = (float)worldY;

		float bestD2 = 1e30f;
		bool found = false;

		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				// CHANGE these if your Rectangle stores position differently
				float cx = cells[i][j].pos.x;
				float cy = cells[i][j].pos.y;

				float dx = mx - cx;
				float dy = my - cy;
				float d2 = dx * dx + dy * dy;

				if (d2 < bestD2)
				{
					bestD2 = d2;
					outI = i;
					outJ = j;
					found = true;
				}
			}
		}

		return found;
	}

	void Grid::RebuildWalkable()
	{
		for (int j = 0; j < MAX_J; ++j)
		{
			for (int i = 0; i < MAX_I; ++i)
			{
				walkable[j * MAX_I + i] = blocked[i][j] ? 0 : 1;
			}
		}
	}

	void Grid::ResetColors()
	{
		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				cells_alpha[i][j] = 1.0f;
				cells_color[i][j] = blocked[i][j] ? 0x222222 : 0xFFFFFF; //BLOCKED = BLACK
			}
		}
	}


	void Grid::PaintStartGoalPath()
	{
		// path cyan
		for (const Cell& c : path)
		{
			if (c.x < 0 || c.x >= MAX_I || c.y < 0 || c.y >= MAX_J) continue;
			cells_color[c.x][c.y] = 0x00FFFF;
		}

		// start green, goal red override
		if (hasStart) cells_color[start_i][start_j] = 0x00FF00;
		if (hasGoal)  cells_color[goal_i][goal_j] = 0xFF0000;
	}

	void Grid::ComputePath()
	{
		path.clear();
		RebuildWalkable();

		if (!hasStart || !hasGoal) return;
		if (blocked[start_i][start_j] || blocked[goal_i][goal_j]) return;

		Cell s{ start_i, start_j };
		Cell g{ goal_i,  goal_j };

		AStarResult res = AStar_FindPath_Grid4(MAX_I, MAX_J, walkable, s, g);
		path = std::move(res.path);
	}

	void Grid::ToggleBlocked(int i, int j)
	{
		if (i < 0 || i >= MAX_I || j < 0 || j >= MAX_J) return;

		// If we're blocking/unblocking on start/goal, remove the marker first
		if (hasStart && i == start_i && j == start_j)
		{
			hasStart = false;
			start_i = start_j = -1;
			path.clear();
		}

		if (hasGoal && i == goal_i && j == goal_j)
		{
			hasGoal = false;
			goal_i = goal_j = -1;
			path.clear();
		}

		blocked[i][j] = !blocked[i][j];
	}

	Grid::Grid()
	{
		for (int i = 0; i < MAX_I; ++i)
		{
			for (int j = 0; j < MAX_J; ++j)
			{
				float cell_x = i * ((float)CELL_WIDTH + offset);
				float cell_y = -(j * ((float)CELL_HEIGHT + offset));


				float display_x = (i - j) * CELL_WIDTH / 2;
				float display_y = -(i + j) * CELL_HEIGHT / 4;
				cells[i][j] = Shape2D::Rectangle(display_x, display_y, CELL_WIDTH, CELL_HEIGHT);
				cells_color[i][j] = 0xFFFFFF;
				cells_alpha[i][j] = 1.0f;
			}
		}
		// init walkable (no walls yet)
		RebuildWalkable();
		ResetColors();
	}


	void Grid::update(float camX, float camY, int winW, int winH)
	{
		int mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);

		float worldX = camX + ((float)mouseX - winW * 0.5f);
		float worldY = camY + (winH * 0.5f - (float)mouseY);

		// Toggle block on the tile under the mouse when B is pressed
		if (AEInputCheckTriggered(AEVK_B))
		{
			int i, j;
			if (PickCellNearest(worldX, worldY, i, j))
			{
				ToggleBlocked(i, j);

				if (hasStart && hasGoal)
					ComputePath();

				ResetColors();
				PaintStartGoalPath();
			}
		}



		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			int i, j;

			if (PickCellNearest(worldX, worldY, i, j))
			{
				if (blocked[i][j]) return; //dont select blocked tile

				// Click 1: set start
				if (!hasStart || (hasStart && hasGoal))
				{
					hasStart = true;
					hasGoal = false;
					start_i = i; start_j = j;
					goal_i = goal_j = -1;
					path.clear();
				}
				// Click 2: set goal, compute path
				else
				{
					hasGoal = true;
					goal_i = i; goal_j = j;

					ComputePath();
				}

				// recolor every click
				ResetColors();
				PaintStartGoalPath();
			}
		}


	}





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

}


void cells_init(Shape2D::Rectangle(&cells)[MAX_I][MAX_J])
{
	for (int i = 0; i < MAX_I; ++i)
	{
		for (int j = 0; j < MAX_J; ++j)
		{
			float x = i * ((float)CELL_WIDTH + offset);
			float y = j * ((float)CELL_HEIGHT + offset);
			cells[i][j] = Shape2D::Rectangle(x, y, CELL_WIDTH, CELL_HEIGHT);
		}
	}
}

//void draw_cells(Shape2D::Rectangle(&cells)[MAX_I][MAX_J])
//{
//	for (int i = 0; i < MAX_I; ++i)
//	{
//		for (int j = 0; j < MAX_J; ++j)
//		{
//			cells[i][j].render();
//		}
//	}
//}