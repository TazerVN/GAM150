#include "game.h"
#include "AEEngine.h"
#include "GameState.h"
#include "Mesh_factory.h"
#include "Shape2D.h"
#include "Grid.h"

float camerax = 800.f;
float cameray = -450.f;

MESH_MODE cur_mode;
Shape2D::Circle c1(0.f, 0.f, 5.0f);
Shape2D::Rectangle r1(0.0f, 0.0f, 100.f, 100.f);
Grid::Grid tile;
s8 pFont; char pText[40];
f32 w, h;

void game_init() {
	//==========(Init)======================
	Shape2D::mesh_initialize();
	cur_mode = MESH_CENTER;
	AEGfxSetCamPosition(camerax, cameray);
	pFont = AEGfxCreateFont("Assets/liberation-mono.ttf", (int)72.f);
	// Text to print
	
	AEGfxGetPrintSize(pFont, pText, 1.f, &w, &h);
}
void game_update() {
	AESysFrameStart();
	//==============(Logic Update)==============
	if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		leaveGameState();
	if (AEInputCheckTriggered(AEVK_F1)) {
		(cur_mode == MESH_CENTER) ? cur_mode = MESH_CORNER : cur_mode = MESH_CENTER;
	}

	f32 dt = (f32)AEFrameRateControllerGetFrameTime();

	int dx = 0, dy = 0;
	f32 spd = 250.0f;

	if (AEInputCheckCurr(AEVK_D)) {
		dx = 1;
	}
	if (AEInputCheckCurr(AEVK_A)) {
		dx = -1;
	}
	if (AEInputCheckCurr(AEVK_W)) {
		dy = 1;
	}
	if (AEInputCheckCurr(AEVK_S)) {
		dy = -1;
	}
	//if direction is pressed
	if (dx != 0 || dy != 0) {
		AEVec2 dir;
		AEVec2Set(&dir, (f32)dx, (f32)dy);
		AEVec2Normalize(&dir, &dir);
		if (dir.x != 0) {
			camerax += dir.x * spd * dt;
			c1.pos.x = camerax;
		}
		if (dir.y != 0) {
			cameray += dir.y * spd * dt;
			c1.pos.y = cameray;
		}
		AEGfxSetCamPosition(camerax, cameray);
		c1.update_tick();
	}

	//==========Object updates===========
	
	tile.update();

	//sprintf(pText,"Camera Pos : %.2f,%.2f",camerax,cameray);
	int suc = sprintf_s(pText,"Camera Pos : %.2f,%.2f", camerax, cameray);

	//========(Render)====================
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	Color::Color_Fill_Hex(0xFF00FF, 1.f);
	r1.render(cur_mode);

	tile.render();

	Color::Color_Fill_Hex(0x000000, 1.0f);
	c1.render();
	
	AEGfxPrint(pFont, pText, 0.f, 0.f, 0.4, 0.f, 0.f, 0.f, 1.f);
	AESysFrameEnd();
}
void game_exit() {
	// free the system
	Shape2D::mesh_free();
}