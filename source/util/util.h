#pragma once
#include "AEEngine.h"
#include "../ECS/ECSystem.h"
#include "../system/TurnBasedSystem.h"
#include "../System/GridSystem.h"

bool point2circle_intersect(float pointX, float pointY, float circleX, float circleY, float diameter);
bool point2rect_intersect(float rx, float ry, float w, float h, float px, float py);
bool rect2rect_intersect(float r1X, float r1Y, float r1W, float r1H, float r2X, float r2Y, float r2W, float r2H);
bool circle2circle_intersect(float c1X, float c1Y, float c1D, float c2X, float c2Y, float c2D);
bool rect2circle_intersect(float rX, float rY, float rW, float rH, float cX, float cY, float cD);