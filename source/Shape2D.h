#pragma once
#include "AEEngine.h"
#include "util.h"
#include "Mesh_factory.h"
#include "renderSystem.h"

#ifndef SHAPE2D
#define SHAPE2D


namespace Color
{
	typedef int ColorHex;
	void Color_Fill_Hex(ColorHex hexcode, float alpha);
};

namespace Shape2D
{

	void mesh_initialize();
	void mesh_free();

	class Shape2D
	{
		public:
		enum ShapeType { circle, rectangle };
		AEVec2 pos;
		AEMtx33 transform;

		void Transform(Shape2D* shape, AEMtx33 transform_matrix);
		Shape2D(f32 x, f32 y);	//constructor

		//void render() override = 0;
	};

	class Rectangle : public Shape2D
	{
		public:
		f32 width, height;
		AEVec2 area;
		Rectangle();
		Rectangle(f32 x, f32 y, f32 w, f32 h);	//constructor
		void update_tick();
		//void render() override;
	};

	class Circle : public Shape2D
	{
		public:
		f32 diameter, radius, scale;
		Circle(f32 x, f32 y, f32 radius);	//constructor
		void update_tick();
		//void render() override;
	};
}
#endif // !SHAPE2D