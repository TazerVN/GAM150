#pragma once
#include "util.h"

bool point2circle_intersect(float pointX, float pointY, float circleX, float circleY, float diameter) {
	float radius = diameter * 0.5f;
	float dx = pointX - circleX;
	float dy = pointY - circleY;
	float distanceSquared = (dx * dx + dy * dy);

	return (distanceSquared < radius * radius) ? 1 : 0;
}

bool point2rect_intersect(float rx, float ry, float w, float h, float px, float py) {
	float minX = rx - w * 0.5f;
	float maxX = rx + w * 0.5f;

	float minY = ry - h * 0.5f;
	float maxY = ry + h * 0.5f;

	return(minX <= px && px <= maxX && minY <= py && py <= maxY) ? 1 : 0;
}

bool rect2rect_intersect(float r1X, float r1Y, float r1W, float r1H, float r2X, float r2Y, float r2W, float r2H) {
	float r1_minX = r1X - r1W * 0.5f;
	float r1_maxX = r1X + r1W * 0.5f;
	float r1_minY = r1Y - r1H * 0.5f;
	float r1_maxY = r1Y + r1H * 0.5f;

	float r2_minX = r2X - r2W * 0.5f;
	float r2_maxX = r2X + r2W * 0.5f;
	float r2_minY = r2Y - r2H * 0.5f;
	float r2_maxY = r2Y + r2H * 0.5f;

	return (r1_minX < r2_maxX && r1_maxX > r2_minX && r1_minY < r2_maxY && r1_maxY > r2_minY) ? 1 : 0;
}

bool circle2circle_intersect(float c1X, float c1Y, float c1D, float c2X, float c2Y, float c2D) {
	float r1 = c1D * 0.5f;
	float r2 = c2D * 0.5f;
	float radii = r1 + r2;

	float dx = c1X - c2X;
	float dy = c1Y - c2Y;
	float distanceSquared = dx * dx + dy * dy;

	return (distanceSquared <= radii * radii) ? 1 : 0;
}

bool rect2circle_intersect(float rX, float rY, float rW, float rH, float cX, float cY, float cD) {
	float minX = rX - rW * 0.5f;
	float maxX = rX + rW * 0.5f;

	float minY = rY - rH * 0.5f;
	float maxY = rY + rH * 0.5f;

	float nearestX = AEClamp(cX, minX, maxX);
	float nearestY = AEClamp(cY, minY, maxY);

	//if nearest point intersect with circle then it is intersecting.
	return (point2circle_intersect(nearestX, nearestY, cX, cY, cD)) ? 1 : 0;
}