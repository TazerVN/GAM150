#pragma once
#include "AEEngine.h"
#include "Shape2D.h"
#include "renderSystem.h"

#define CELL_WIDTH 128
#define CELL_HEIGHT 128

#define MAX_I 16
#define MAX_J 9

namespace Grid
{
	class Grid : public RenderSystem::RenderObject
	{
		public:
		Shape2D::Rectangle cells[MAX_I][MAX_J];
		int cells_color[MAX_I][MAX_J];
		f32 cells_alpha[MAX_I][MAX_J];
		Grid();		//default constructor
		void update();
		void render() override;
	};
}

void cells_init(Shape2D::Rectangle(&cells)[MAX_I][MAX_J]);
void draw_cells(Shape2D::Rectangle(&cells)[MAX_I][MAX_J]);