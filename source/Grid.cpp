#include "Grid.h"
#include "Shape2D.h"

float offset = 1.0f;

namespace Grid {
	Grid::Grid() {
		//cells_init(cells);		//(old code)
		for (int i = 0; i < MAX_I; ++i) {
			for (int j = 0; j < MAX_J; ++j) {
				float cell_x = i * ((float)CELL_WIDTH + offset);
				float cell_y = -(j * ((float)CELL_HEIGHT + offset));
				cells[i][j] = Shape2D::Rectangle(cell_x, cell_y, CELL_WIDTH, CELL_HEIGHT);
				cells_color[i][j] = 0x00FF00;	//green
				cells_alpha[i][j] = 1.0f;
			}
		}
	}
	void Grid::update() {
		int mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX,&mouseY);

	}
	void Grid::render() {
		//draw_cells(cells);		//(old code)
		for (int i = 0; i < MAX_I; ++i) {
			for (int j = 0; j < MAX_J; ++j) {
				Color::Color_Fill_Hex(cells_color[i][j], cells_alpha[i][j]);
				cells[i][j].render(MESH_CORNER);
			}
		}
	}
}


void cells_init(Shape2D::Rectangle(&cells)[MAX_I][MAX_J]){
	for (int i = 0; i < MAX_I; ++i) {
		for (int j = 0; j < MAX_J; ++j) {
			float x = i * ((float)CELL_WIDTH + offset);
			float y = j * ((float)CELL_HEIGHT + offset);
			cells[i][j] = Shape2D::Rectangle(x, y, CELL_WIDTH, CELL_HEIGHT);
		}
	}
}

void draw_cells(Shape2D::Rectangle(&cells)[MAX_I][MAX_J]) {
	for (int i = 0; i < MAX_I; ++i) {
		for (int j = 0; j < MAX_J; ++j) {
			cells[i][j].render(MESH_CORNER);
		}
	}
}