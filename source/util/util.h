//===============================================================
//	AUTHOR:		Wai Phyoo Ooo
// 
//	EMAIL:		w.phyooo@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once
#include "AEEngine.h"

int math_absolute(int);
int math_max(int const&, int const&);
bool point2circle_intersect(float pointX, float pointY, float circleX, float circleY, float diameter);
bool point2rect_intersect(float rx, float ry, float w, float h, float px, float py);
bool rect2rect_intersect(float r1X, float r1Y, float r1W, float r1H, float r2X, float r2Y, float r2W, float r2H);
bool circle2circle_intersect(float c1X, float c1Y, float c1D, float c2X, float c2Y, float c2D);
bool rect2circle_intersect(float rX, float rY, float rW, float rH, float cX, float cY, float cD);

void translate_To_Isometric(AEVec2 offset, f32 height, f32& x, f32& y, s32 grid_x, s32 grid_y);

s32 grid_dist_manhattan(s32 const& x1, s32 const& y1, s32 const& x2, s32 const& y2);

s32 grid_dist_chebyshev(s32 const& x1, s32 const& y1, s32 const& x2, s32 const& y2);