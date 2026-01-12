#include "Shape2D.h"

namespace Color {
	void Color_Fill_Hex(ColorHex hexcode, float alpha) {

		if (hexcode > 0xFFFFFF || hexcode < 0x000000) return;
		float r, g, b;

		int temp = hexcode;

		b = (float)(temp % 0x100) / 255.0f;
		temp /= 0x100;

		g = (float)(temp % 0x100) / 255.0f;
		temp /= 0x100;

		r = (float)(temp % 0x100) / 255.0f;
		temp /= 0x100;

		AEGfxSetColorToMultiply(r, g, b, alpha);
	}
}

namespace Shape2D {
	//======================(Mesh)===========================
	static AEGfxVertexList* rectMesh_center = 0;
	static AEGfxVertexList* rectMesh_corner = 0;
	static AEGfxVertexList* cirMesh = 0;
	static AEGfxVertexList* lineMesh = 0;

	void mesh_initialize() {
		if (!rectMesh_center) {
			rectMesh_center = createMesh_triangular_rectangle(MESH_CENTER);
		}
		if (!rectMesh_corner) {
			rectMesh_corner = createMesh_triangular_rectangle(MESH_CORNER);
 		}
		if (!cirMesh) {
			cirMesh = createMesh_triangular_circle();
		}
		if (!lineMesh) {
			lineMesh = createMesh_line();
		}
	}

	void mesh_free() {
		if(rectMesh_center != nullptr)AEGfxMeshFree(rectMesh_center);
		if (rectMesh_center != nullptr)AEGfxMeshFree(rectMesh_corner);
		if(cirMesh != nullptr)AEGfxMeshFree(cirMesh);
		if (lineMesh != nullptr)AEGfxMeshFree(lineMesh);
	}

	//=================(Shapes)============================
	//===================(Base)=========================
	void Shape2D::Transform(Shape2D* shape, AEMtx33 transform_matrix) {
		AEMtx33Concat(&shape->transform, &shape->transform, &transform_matrix);
	}
	Shape2D::Shape2D(f32 x, f32 y) {
		AEVec2Set(&this->pos, x, y);
		AEMtx33Identity(&this->transform);
	}
	//================(Rectangle)=======================
	Rectangle::Rectangle() : Shape2D(0.f,0.f) {
		this->width = 0.f;
		this->height = 0.f;
		AEVec2Set(&this->area, 0.f, 0.f);
		Rectangle::update_tick();
	}
	Rectangle::Rectangle(f32 x, f32 y, f32 w, f32 h) : Shape2D(x, y) {
		this->width = w;
		this->height = h;
		AEVec2Set(&this->area, w, h);
		Rectangle::update_tick();
	}

	void Rectangle::update_tick() {
		AEMtx33 translate;
		AEMtx33Trans(&translate, this->pos.x, this->pos.y);
		AEMtx33 scale;
		AEMtx33Scale(&scale, this->area.x, this->area.y);
		AEMtx33Concat(&this->transform, &translate, &scale);
	}

	void Rectangle::render(MESH_MODE mode) {
		AEGfxSetTransform(this->transform.m);
		if(mode == MESH_CENTER) AEGfxMeshDraw(rectMesh_center, AE_GFX_MDM_TRIANGLES);
		else if(mode == MESH_CORNER) AEGfxMeshDraw(rectMesh_corner, AE_GFX_MDM_TRIANGLES);
	}
	//================(Circle)=======================
	Circle::Circle(f32 x, f32 y, f32 radius) : Shape2D(x, y) {
		this->radius = radius;
		this->diameter = 2 * radius;
		this->scale = this->radius / 0.5f; //original model radius was 0.5f
		Circle:: update_tick();
	}
	void Circle::update_tick() {
		AEMtx33 translate;
		AEMtx33Trans(&translate, this->pos.x, this->pos.y);
		AEMtx33 scale;
		AEMtx33Scale(&scale, this->scale, this->scale);
		AEMtx33Concat(&this->transform, &translate, &scale);
	}
	void Circle::render() {
		AEGfxSetTransform(this->transform.m);
		AEGfxMeshDraw(cirMesh, AE_GFX_MDM_TRIANGLES);
	}
}