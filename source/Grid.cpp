#include "Grid.h"
#include "Shape2D.h"

float offset = 1.0f;

namespace Grid
{
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
	}
	void Grid::update()
	{
		int mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);

	}

	void Grid::render()
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
	}

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

void draw_cells(Shape2D::Rectangle(&cells)[MAX_I][MAX_J])
{
	for (int i = 0; i < MAX_I; ++i)
	{
		for (int j = 0; j < MAX_J; ++j)
		{
			cells[i][j].render();
		}
	}
}